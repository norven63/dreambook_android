package cn.retech.custom_control;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import android.animation.LayoutTransition;
import android.animation.LayoutTransition.TransitionListener;
import android.content.ClipData;
import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.view.DragEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import cn.retech.activity.MyApplication;
import cn.retech.activity.R;
import cn.retech.custom_control.AnimatorDirector.AnimatorBuilder;
import cn.retech.domainbean_model.book.BookFile;
import cn.retech.global_data_cache.GlobalDataCacheForMemorySingleton;

public class DragGridLayout extends AutoColumnGridLayout {
	public interface HandleActivityEvent {
		public void onViewItemLongClick();

		public void onHandleEnter();
	}

	private Handler handler;

	private boolean hasChangedHeight = false;// 判断是否已动态拉长过,如果是的话则需要动态缩短长度
	private boolean isDragOutOfGridView = false;// 判断是否超出屏幕
	private float lastDragX = 0;
	private boolean isDragFromLeft = true;

	private OnLongClickListener viewItemOnLongClickListener;
	private HandleActivityEvent handleActivityEvent;
	private OnDragListener viewItemDragListener;
	private OnDragListener viewGroupDragListener;
	private OnClickListener folderOnClickListener;

	public void setOnLongClickListenerFromActivity(HandleActivityEvent HandleActivityEvent) {
		this.handleActivityEvent = HandleActivityEvent;
	}

	public void setOnFolderClickListener(OnClickListener onFolderClickListener) {
		this.folderOnClickListener = onFolderClickListener;
	}

	// 为了防止ViewGroup和View同时监听drag事件而起冲突:true-ViewGroup可监听;false-View监听
	private boolean isViewItemHandleDragEvent = false;

	public DragGridLayout(Context context) {
		super(context);
		init();
	}

	public DragGridLayout(Context context, AttributeSet attrs) {
		super(context, attrs);
		init();
	}

	public DragGridLayout(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		init();
	}

	@Override
	public void addView(View view, int index, android.view.ViewGroup.LayoutParams params) {
		super.addView(view, index, params);

		view.setOnLongClickListener(viewItemOnLongClickListener);
		view.setOnDragListener(viewItemDragListener);
		view.setTag(R.id.timer, new Timer());
		view.setTag(R.id.isPreparing, false);

		// 如果是文件夹类型,则不仅需要为其单独额外添加一个onFolderClickListener,还需要为其所有的子元素依次初始化相应数据
		if (view instanceof BookFolderLayout) {
			view.setOnClickListener(folderOnClickListener);

			Iterator<View> iterator = ((BookFolderLayout) view).getChildViewsIterator();
			while (iterator.hasNext()) {
				View childView = iterator.next();

				childView.setOnClickListener(onItemClickListener);
				childView.setOnLongClickListener(viewItemOnLongClickListener);
				childView.setOnDragListener(viewItemDragListener);
				childView.setTag(R.id.timer, new Timer());
				childView.setTag(R.id.isPreparing, false);
			}
		} else {
			view.setOnClickListener(onItemClickListener);
		}
	}

	private void init() {
		getLayoutTransition().addTransitionListener(new TransitionListener() {
			@Override
			public void startTransition(LayoutTransition transition, ViewGroup container, View view, int transitionType) {
			}

			/**
			 * 该段代码的逻辑是：当书架网格上的最后一本书的（bottom+50）值比网格的要大时，或者在已经执行过动态拉长操作之后时， 则动态拉长书架网格的高度至一个合适的值
			 */
			@Override
			public void endTransition(LayoutTransition transition, ViewGroup container, View view, int transitionType) {
				ViewGroup parentGroup = (ViewGroup) DragGridLayout.this.getParent();

				View lastView = DragGridLayout.this.getChildAt(getChildCount() - 1);

				if (null == lastView) {
					return;
				}

				/**
				 * 如果满足动态拉长条件则执行拉长操作 (之所以用一个boolean标记位来控制是否需要缩短是因为考虑到首次加载界面的情况)
				 */
				LinearLayout.LayoutParams layoutParams = (LinearLayout.LayoutParams) getLayoutParams();
				if (lastView.getBottom() + 70 >= DragGridLayout.this.getBottom()) {
					layoutParams.height = lastView.getBottom() + 50;
					parentGroup.updateViewLayout(DragGridLayout.this, layoutParams);

					hasChangedHeight = true;// 在执行过拉长操作后将此标记为设为true
				} else if (hasChangedHeight) {
					int newHeight = 0;

					// 将最后一个子元素的bottom值与屏幕的高度值相比较，取较大者作为新的高度值
					if (lastView.getBottom() + 70 > getResources().getDisplayMetrics().heightPixels) {
						newHeight = lastView.getBottom();
					} else {
						newHeight = getResources().getDisplayMetrics().heightPixels;
					}

					layoutParams.height = newHeight + 70;
					parentGroup.updateViewLayout(DragGridLayout.this, layoutParams);
				}
			}
		});

		handler = new Handler() {
			@Override
			public void handleMessage(Message msg) {
				DragMessageBean dragMessageBean = (DragMessageBean) msg.obj;
				int listenerViewIndex = msg.arg1;
				ViewGroup parentView = dragMessageBean.getViewGroup();
				final View listenerView = dragMessageBean.getListenerView();
				final View dragView = dragMessageBean.getDragView();

				/**
				 * 能够触发可位移操作的两个条件: 1.停留超过指定秒数后并继续拖动,根据左移入还是右移入来判断当坐标与"listenerView的2分之1宽度值"的值 2.dragView是一个CellView
				 */
				if (msg.what == 1 || dragView instanceof BookFolderLayout) {
					// 为了防止listenerView在执行布局调整动画时仍然继续监听onDrag事件,并然后在0.35秒后恢复其监听功能
					listenerView.setTag(R.id.isMoving, true);
					new Timer().schedule(new TimerTask() {
						@Override
						public void run() {
							handler.post(new Runnable() {
								@Override
								public void run() {
									listenerView.setTag(R.id.isMoving, false);
									listenerView.setTag(R.id.isPreparing, false);

									AnimatorDirector.animate(new AnimatorBuilder().view(listenerView).scaleX(1.0f).scaleY(1.0f));

									// 如果是文件夹,还需要还原封面背景图
									if (listenerView instanceof BookFolderLayout) {
										listenerView.findViewById(R.id.book_folder_gridLayout).setBackgroundResource(R.drawable.bg_folder);
									}
								}
							});
						}
					}, 350);

					if (null != dragView.getTag()) {
						BookFolderLayout bookFolderLayout = (BookFolderLayout) dragView.getTag();
						bookFolderLayout.removeChildView(dragView, true);
					}

					if (null != ((ViewGroup) dragView.getParent())) {
						((ViewGroup) dragView.getParent()).removeView(dragView);
					}

					parentView.addView(dragView, listenerViewIndex);

					// 结构层操作
					List<BookFile> rootBookFileLists = GlobalDataCacheForMemorySingleton.getInstance.getRootDirectory().getListFiles();
					BookFile bookFile = ((DragView) dragView).getBookFile();
					if (rootBookFileLists.contains(bookFile)) {
						rootBookFileLists.remove(bookFile);
					}
					rootBookFileLists.add(listenerViewIndex, bookFile);

					// 强制执行一次布局动画,使子元素能够正常排列(防止错乱现象)
					((ViewGroup) dragView.getParent()).scheduleLayoutAnimation();

				} else {
					/**
					 * 不满足移动条件时,则触发放大动画,表示可合并
					 */
					AnimatorDirector.animate(new AnimatorBuilder().view(listenerView).scaleX(1.1f).scaleY(1.1f));
					listenerView.setTag(R.id.isPreparing, true);// 表示正处于准备状态,可以随时发生交互行为
					// 如果是文件夹,还需要更换封面背景图
					if (listenerView instanceof BookFolderLayout) {
						listenerView.findViewById(R.id.book_folder_gridLayout).setBackgroundResource(R.drawable.bg_folder_ondrag);
					}
				}
			}
		};

		viewItemOnLongClickListener = new OnLongClickListener() {
			@Override
			public boolean onLongClick(View view) {
				// 设置数据,会被拖动目标ViewGroup所接收,即用来传递信息
				ClipData data = ClipData.newPlainText("", "");

				// 设置拖动阴影,即你所拖动的那个图标(这也同时说明,你拖动的不过是一个影分身,本尊其实并没有移动)
				DragShadowBuilder shadowBuilder = new View.DragShadowBuilder(view);
				view.startDrag(data, shadowBuilder, view, 0);

				((DragView) view).hiddenSelf();

				isViewItemHandleDragEvent = false;

				if (handleActivityEvent != null) {
					handleActivityEvent.onViewItemLongClick();
				}

				return true;
			}
		};

		// 每个单独View的拖动监听
		viewItemDragListener = new OnDragListener() {
			@Override
			public boolean onDrag(final View listenerView, DragEvent event) {
				// 如果listenerView正在执行一个移动动画则直接返回false
				if (null != listenerView.getTag(R.id.isMoving) && (Boolean) listenerView.getTag(R.id.isMoving)) {
					return false;
				}

				final View dragView = (View) event.getLocalState();

				// 如果拖动事件遮蔽自己则直接返回false
				if (listenerView == dragView) {
					return false;
				}

				final MyGridLayout parentView = (MyGridLayout) listenerView.getParent();
				if (null == parentView) {
					return false;
				}

				final int listenerViewIndex = parentView.indexOfChild(listenerView);

				Timer timer = (Timer) listenerView.getTag(R.id.timer);

				switch (event.getAction()) {
				case DragEvent.ACTION_DRAG_ENTERED:
					handleActivityEvent.onHandleEnter();

					// 开启计时器,在只有当dragView在listenerView处停留0.25秒后才会有相应效果
					timer.schedule(new TimerTask() {
						@Override
						public void run() {
							handler.post(new Runnable() {
								@Override
								public void run() {
									DragMessageBean dragMessageBean = new DragMessageBean();
									dragMessageBean.setViewGroup(parentView);
									dragMessageBean.setListenerView(listenerView);
									dragMessageBean.setDragView(dragView);

									// 发送Message以触发Handle处理,其中包含ViewGroupAnddRemove以及listenerView的索引
									Message message = new Message();
									message.obj = dragMessageBean;
									message.arg1 = listenerViewIndex;// listenerView索引

									handler.sendMessage(message);
								}
							});
						}
					}, 250);

					break;
				case DragEvent.ACTION_DRAG_LOCATION:
					/**
					 * 触发可以移动的两个条件之1:当listenerView处于可以操作状态(即停留了0.25秒,这里采用alpha值来判断,因为停留0 .25 秒后listenerView会执行闪烁动画, 改变alpha值。 但这不是一个可靠的参数,建议专门使用一个标记位来进行标识),并且继续移动超过了"listenerView的2分之1宽度值"
					 */
					boolean canListenerViewMove = false;// 标记是否超过了"listenerView的2分之1宽度值"

					// 这个isLeft的赋值操作在ViewGroup的拖动监听中执行
					if (isDragFromLeft) {
						canListenerViewMove = event.getX() >= listenerView.getWidth() * 3 / 4;
					} else {
						canListenerViewMove = event.getX() <= listenerView.getWidth() / 4;
					}

					if ((Boolean) listenerView.getTag(R.id.isPreparing) && canListenerViewMove) {
						DragMessageBean dragMessageBean = new DragMessageBean();
						dragMessageBean.setViewGroup(parentView);
						dragMessageBean.setListenerView(listenerView);
						dragMessageBean.setDragView(dragView);

						Message message = new Message();
						message.obj = dragMessageBean;
						message.arg1 = listenerViewIndex;
						message.what = 1;// 直接设置为1,当handler处理时,将直接判定为能够触发可位移操作

						handler.sendMessage(message);
					}

					break;
				case DragEvent.ACTION_DRAG_EXITED:
					listenerView.setTag(R.id.isPreparing, false);
					// 还原大小
					AnimatorDirector.animate(new AnimatorBuilder().view(listenerView).scaleX(1.0f).scaleY(1.0f));

					// 如果是文件夹,还需要还原封面背景图
					if (listenerView instanceof BookFolderLayout) {
						listenerView.findViewById(R.id.book_folder_gridLayout).setBackgroundResource(R.drawable.bg_folder);
					}

					// 取消定时器,并重置
					timer.cancel();
					listenerView.setTag(R.id.timer, new Timer());

					break;
				case DragEvent.ACTION_DROP:
					// 取消定时器,并重置
					timer.cancel();
					listenerView.setTag(R.id.timer, new Timer());

					if ((Boolean) listenerView.getTag(R.id.isPreparing)) {
						listenerView.setTag(R.id.isPreparing, false);

						// 还原大小
						AnimatorDirector.animate(new AnimatorBuilder().view(listenerView).scaleX(1.0f).scaleY(1.0f));

						// 如果是文件夹,还需要还原封面背景图
						if (listenerView instanceof BookFolderLayout) {
							listenerView.findViewById(R.id.book_folder_gridLayout).setBackgroundResource(R.drawable.bg_folder);
						}

						BookFile rootBookFile = GlobalDataCacheForMemorySingleton.getInstance.getRootDirectory();
						if (listenerView instanceof BookFolderLayout) {
							if (((BookFolderLayout) listenerView).addChildView(0, dragView)) {
								// 结构层操作
								((DragView) listenerView).getBookFile().getListFiles().add(0, ((DragView) dragView).getBookFile());
							}
						} else {
							BookFolderLayout newFolder = new BookFolderLayout(getContext());

							// 结构层操作
							List<BookFile> bookFiles = new ArrayList<BookFile>();
							bookFiles.add(((DragView) listenerView).getBookFile());
							bookFiles.add(((DragView) dragView).getBookFile());
							BookFile newBookFolder = BookFile.createDirectoryWithName(rootBookFile.createValidNewDirectoryNameInCurrentlyDirectory(), bookFiles);
							newFolder.setBookFile(newBookFolder);

							if (listenerViewIndex >= parentView.getChildCount()) {
								parentView.addView(newFolder, 0);

								rootBookFile.getListFiles().add(0, newBookFolder);// 结构层操作
							} else {
								parentView.addView(newFolder, listenerViewIndex);

								rootBookFile.getListFiles().add(listenerViewIndex, newBookFolder);// 结构层操作
							}

							// 结构层操作
							rootBookFile.getListFiles().remove(((DragView) listenerView).getBookFile());
							rootBookFile.getListFiles().remove(((DragView) dragView).getBookFile());

							newFolder.addChildView(listenerView);
							newFolder.addChildView(dragView);
						}

						isViewItemHandleDragEvent = true;
					} else {
						((DragView) dragView).showSelf();
					}

					break;
				case DragEvent.ACTION_DRAG_ENDED:
					listenerView.setTag(R.id.isPreparing, false);

					AnimatorDirector.animate(new AnimatorBuilder().view(listenerView).scaleX(1.0f).scaleY(1.0f));

					// 判断是否被拖出Activity之外,如果是则将dragView复原
					if (isDragOutOfGridView) {
						((DragView) dragView).showSelf();
					}

					break;
				}

				return true;
			}
		};

		// ViewGroup的拖动监听
		viewGroupDragListener = new OnDragListener() {
			@Override
			public boolean onDrag(View view, DragEvent event) {
				final View dragView = (View) event.getLocalState();
				switch (event.getAction()) {
				case DragEvent.ACTION_DROP:
					dragView.setOnDragListener(viewItemDragListener);

					float e_x = event.getX() - dragView.getWidth() / 2;
					float e_y = event.getY() - dragView.getHeight() / 2;

					if (!isViewItemHandleDragEvent) {
						((DragView) dragView).showSelf();

						if (null != dragView.getTag()) {
							/**
							 * 这里是dragView从一个文件夹拖动至桌面的情况分支
							 */
							BookFolderLayout folder = (BookFolderLayout) dragView.getTag();
							folder.removeChildView(dragView, true);

							BookShelfBookCell newBookCell = new BookShelfBookCell(MyApplication.getApplication());
							newBookCell.bind(((DragView) dragView).getBookFile());

							DragGridLayout.this.addView(newBookCell, 0);

							DragGridLayout.this.refreshDrawableState();

							// 结构层操作
							BookFile rootBookFile = GlobalDataCacheForMemorySingleton.getInstance.getRootDirectory();
							rootBookFile.getListFiles().add(0, ((DragView) dragView).getBookFile());
						} else {
							/**
							 * 这里是dragView处于桌面拖动的情况分支。只须执行从拖动点移回至原位的动画即可。
							 */
							float x = dragView.getX();
							float y = dragView.getY();

							dragView.setX(e_x);
							dragView.setY(e_y);

							AnimatorDirector.animate(new AnimatorBuilder().view(dragView).x(x).y(y).setDuration(350));
						}
					}

					break;
				case DragEvent.ACTION_DRAG_ENTERED:
					isDragOutOfGridView = false;

					// 记录拖动开始位置坐标,并隐藏文件夹界面
					lastDragX = event.getX();

					break;
				case DragEvent.ACTION_DRAG_LOCATION:
					// 判断是从左移还是从右移,这个值将会在viewItemDragListener中判断是否可触发移动效果时用到
					float currentDragX = event.getX();

					if (currentDragX > lastDragX) {
						isDragFromLeft = true;
					} else {
						isDragFromLeft = false;
					}

					lastDragX = event.getX();

					break;
				case DragEvent.ACTION_DRAG_EXITED:
					isDragOutOfGridView = true;

					break;
				case DragEvent.ACTION_DRAG_ENDED:
					// 判断是否被拖出Activity之外,如果是则将dragView的状态复原
					if (isDragOutOfGridView) {
						((DragView) dragView).showSelf();
					}

					break;
				}

				return true;
			}
		};
		this.setOnDragListener(viewGroupDragListener);
	}
}
