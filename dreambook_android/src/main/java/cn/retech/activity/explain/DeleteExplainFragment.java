package cn.retech.activity.explain;

import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout.LayoutParams;
import cn.retech.activity.R;
import cn.retech.custom_control.AnimatorDirector;
import cn.retech.custom_control.AnimatorDirector.AnimatorBuilder;

public class DeleteExplainFragment extends ExplainFragment {
	public DeleteExplainFragment() {
		super();
	}

	private ImageView deleteView;
	private ImageView moveBook;

	private float differenceX;
	private float differenceY;
	private float deleteViewX;
	private float deleteViewY;

	private final ICurrentState firstState = new ICurrentState() {
		@Override
		public void animation() {
			// 计算坐标差量
			differenceX = moveBook.getX() - (deleteViewX + deleteView.getWidth() / 2);
			differenceY = moveBook.getY() - (deleteViewY - moveBook.getHeight() / 2);

			// 通过移动书籍的前后坐标差量算出手指和触电的目的坐标
			staticViewStateMap.put(StaticViewStateKeyEnum.HAND_X, hand.getX() - differenceX);
			staticViewStateMap.put(StaticViewStateKeyEnum.HAND_Y, hand.getY() - differenceY);

			staticViewStateMap.put(StaticViewStateKeyEnum.TOUCH_POINT_X, touchPoint.getX() - differenceX);
			staticViewStateMap.put(StaticViewStateKeyEnum.TOUCH_POINT_Y, touchPoint.getY() - differenceY);

			staticViewStateMap.put(StaticViewStateKeyEnum.MOVEBOOK_X, deleteViewX + deleteView.getWidth() / 2);
			staticViewStateMap.put(StaticViewStateKeyEnum.MOVEBOOK_Y, deleteViewY - moveBook.getHeight() * 2 / 3);

			staticViewStateMap.put(StaticViewStateKeyEnum.DELETEVIEW_X, deleteViewX);
			staticViewStateMap.put(StaticViewStateKeyEnum.DELETEVIEW_Y, deleteViewY);

			AnimatorDirector.animate(new AnimatorBuilder().view(moveBook).scaleX(1.2f).scaleY(1.2f).setDuration(animatorTime / 2));

			needCancelAnimatorSets.add(AnimatorDirector.animate(new AnimatorBuilder().view(deleteView).x(deleteViewX).y(deleteViewY)
					.setDuration(animatorTime / 2).setListener(new AnimatorFinishListener())));
		}
	};

	private final ICurrentState secondState = new ICurrentState() {
		@Override
		public void animation() {
			AnimatorDirector.animate(new AnimatorBuilder().view(moveBook).x(deleteViewX + deleteView.getWidth() / 2)
					.y(deleteViewY - moveBook.getHeight() / 2).setDuration(animatorTime).setDuration(animatorTime));

			AnimatorDirector.animate(new AnimatorBuilder().view(hand).x(hand.getX() - differenceX).y(hand.getY() - differenceY)
					.setDuration(animatorTime));

			needCancelAnimatorSets.add(AnimatorDirector.animate(new AnimatorBuilder().view(touchPoint).x(touchPoint.getX() - differenceX)
					.y(touchPoint.getY() - differenceY).setDuration(animatorTime).setListener(new AnimatorFinishListener())));
		}
	};

	private final ICurrentState thirdState = new ICurrentState() {
		@Override
		public void animation() {
			moveBook.setAlpha(0.8f);
			deleteView.setImageResource(R.drawable.ic_delete_selected);
		}
	};

	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);

		deleteView = (ImageView) getView().findViewById(R.id.explain_delete_imageView);

		moveBook = (ImageView) getView().findViewById(R.id.explain_moveBook_imageView);
	}

	@Override
	protected void initStates() {
		states.addFirst(thirdState);
		states.addFirst(secondState);
		states.addFirst(firstState);
	}

	@Override
	public void startAnimation() {
		anchorView = (ImageView) gridView.getAdapter().getItem(5);
		anchorView.setImageResource(R.drawable.ic_explain_dynamic_book);

		super.startAnimation();
	}

	@Override
	protected void initiativeLayout() {
		super.initiativeLayout();

		deleteViewX = deleteView.getX();
		deleteViewY = deleteView.getY();

		deleteView.setX(deleteViewX - deleteView.getWidth());
		deleteView.setY(deleteViewY + deleteView.getHeight());
		deleteView.setVisibility(View.VISIBLE);

		anchorView.setVisibility(View.INVISIBLE);
		LayoutParams layoutParams = (LayoutParams) moveBook.getLayoutParams();
		layoutParams.width = getResources().getInteger(R.integer.moveBook_width);
		layoutParams.height = getResources().getInteger(R.integer.moveBook_height);
		moveBook.setLayoutParams(layoutParams);
		moveBook.setX(anchorView.getX() + 3);
		moveBook.setY(anchorView.getY() + titleBar.getHeight() + 3);
		moveBook.setVisibility(View.VISIBLE);
	}

	@Override
	protected void finallyLayout() {
		super.finallyLayout();
		anchorView.setVisibility(View.INVISIBLE);
		LayoutParams layoutParams = (LayoutParams) moveBook.getLayoutParams();
		layoutParams.width = getResources().getInteger(R.integer.moveBook_width);
		layoutParams.height = getResources().getInteger(R.integer.moveBook_height);
		moveBook.setLayoutParams(layoutParams);
		moveBook.setScaleX(1.2f);
		moveBook.setScaleY(1.2f);
		moveBook.setAlpha(0.7f);
		moveBook.setX((Float) staticViewStateMap.get(StaticViewStateKeyEnum.MOVEBOOK_X));
		moveBook.setY((Float) staticViewStateMap.get(StaticViewStateKeyEnum.MOVEBOOK_Y));
		moveBook.setVisibility(View.VISIBLE);

		deleteView.setX((Float) staticViewStateMap.get(StaticViewStateKeyEnum.DELETEVIEW_X));
		deleteView.setY((Float) staticViewStateMap.get(StaticViewStateKeyEnum.DELETEVIEW_Y));
		deleteView.setImageResource(R.drawable.ic_delete_selected);
		deleteView.setVisibility(View.VISIBLE);
	}
}
