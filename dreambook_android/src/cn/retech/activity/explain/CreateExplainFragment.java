package cn.retech.activity.explain;

import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout.LayoutParams;
import cn.retech.activity.R;
import cn.retech.custom_control.AnimatorDirector;
import cn.retech.custom_control.AnimatorDirector.AnimatorBuilder;

public class CreateExplainFragment extends ExplainFragment {
	public CreateExplainFragment() {
		super();
	}

	private ImageView anchorView2;
	private ImageView combineBook;
	private ImageView moveBook;

	private float differenceX;
	private float differenceY;

	private final ICurrentState firstState = new ICurrentState() {
		@Override
		public void animation() {
			// 计算坐标差量
			differenceX = moveBook.getX() - (anchorView2.getX() + anchorView2.getWidth() / 2);
			differenceY = moveBook.getY() - (anchorView2.getY() + anchorView2.getHeight() / 2 + titleBar.getHeight());

			// 通过移动书籍的前后坐标差量算出手指和触电的目的坐标
			staticViewStateMap.put(StaticViewStateKeyEnum.HAND_X, hand.getX() - differenceX);
			staticViewStateMap.put(StaticViewStateKeyEnum.HAND_Y, hand.getY() - differenceY);

			staticViewStateMap.put(StaticViewStateKeyEnum.TOUCH_POINT_X, touchPoint.getX() - differenceX);
			staticViewStateMap.put(StaticViewStateKeyEnum.TOUCH_POINT_Y, touchPoint.getY() - differenceY);

			needCancelAnimatorSets.add(AnimatorDirector.animate(new AnimatorBuilder().view(moveBook).scaleX(1.2f).scaleY(1.2f)
					.setDuration(animatorTime / 2).setListener(new AnimatorFinishListener())));
		}
	};

	private final ICurrentState secondState = new ICurrentState() {
		@Override
		public void animation() {
			AnimatorDirector.animate(new AnimatorBuilder().view(moveBook).x(anchorView2.getX() + anchorView2.getWidth() / 2)
					.y(anchorView2.getY() + anchorView2.getHeight() / 2 + titleBar.getHeight()).setDuration(animatorTime).setDuration(animatorTime));

			AnimatorDirector.animate(new AnimatorBuilder().view(hand).x(hand.getX() - differenceX).y(hand.getY() - differenceY)
					.setDuration(animatorTime));

			needCancelAnimatorSets.add(AnimatorDirector.animate(new AnimatorBuilder().view(touchPoint).x(touchPoint.getX() - differenceX)
					.y(touchPoint.getY() - differenceY).setDuration(animatorTime).setListener(new AnimatorFinishListener())));
		}
	};

	private final ICurrentState thirdState = new ICurrentState() {
		@Override
		public void animation() {
			combineBook.setImageResource(R.drawable.ic_explain_dynamic_book);
			anchorView2.setVisibility(View.VISIBLE);
			needCancelAnimatorSets.add(AnimatorDirector.animate(new AnimatorBuilder().view(anchorView2).scaleX(1.2f).scaleY(1.2f)
					.setDuration(animatorTime / 3).setListener(new AnimatorFinishListener())));
		}
	};

	private final ICurrentState fourthState = new ICurrentState() {
		@Override
		public void animation() {
			AnimatorDirector.animate(new AnimatorBuilder().view(touchPoint).alpha(0f).setDuration(animatorTime / 3));

			AnimatorDirector.animate(new AnimatorBuilder().view(anchorView2).scaleX(1f).scaleY(1f).setDuration(animatorTime / 2));

			float redress = Float.valueOf(getResources().getString(R.string.redress));

			anchorView2.setImageResource(android.R.color.white);

			AnimatorDirector.animate(new AnimatorBuilder().view(combineBook)
					.x(anchorView2.getX() - anchorView2.getWidth() / 3 + anchorView2.getWidth() * redress)
					.y(anchorView2.getY() - anchorView2.getHeight() / 4 + titleBar.getHeight() + anchorView2.getHeight() * redress).scaleX(0.4f)
					.scaleY(0.4f).setDuration(animatorTime / 2));

			AnimatorDirector.animate(new AnimatorBuilder().view(moveBook)
					.x(anchorView2.getX() - anchorView2.getWidth() / 3 + anchorView2.getWidth() / 2 + anchorView2.getWidth() * redress)
					.y(anchorView2.getY() - anchorView2.getHeight() / 4 + titleBar.getHeight() + anchorView2.getHeight() * redress).scaleX(0.4f)
					.scaleY(0.4f).setDuration(animatorTime / 2));
		}
	};

	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);

		combineBook = (ImageView) getView().findViewById(R.id.explain_combineBook_imageView);
		moveBook = (ImageView) getView().findViewById(R.id.explain_moveBook_imageView);
	}

	@Override
	protected void initStates() {
		states.addFirst(fourthState);
		states.addFirst(thirdState);
		states.addFirst(secondState);
		states.addFirst(firstState);
	}

	@Override
	public void startAnimation() {
		anchorView = (ImageView) gridView.getAdapter().getItem(8);
		anchorView.setImageResource(R.drawable.ic_explain_dynamic_book);

		super.startAnimation();
	}

	private void changeLayoutParams(View view, View targetView) {
		LayoutParams layoutParams = (LayoutParams) view.getLayoutParams();
		layoutParams.width = getResources().getInteger(R.integer.moveBook_width);
		layoutParams.height = getResources().getInteger(R.integer.moveBook_height);
		view.setLayoutParams(layoutParams);
		view.setX(targetView.getX() + 8);
		view.setY(targetView.getY() + titleBar.getHeight() + 8);
		view.setVisibility(View.VISIBLE);
	}

	@Override
	protected void initiativeLayout() {
		super.initiativeLayout();

		anchorView.setVisibility(View.INVISIBLE);

		anchorView2 = (ImageView) gridView.getAdapter().getItem(3);
		anchorView2.setVisibility(View.INVISIBLE);
		anchorView2.setImageResource(android.R.color.darker_gray);

		changeLayoutParams(combineBook, anchorView2);
		changeLayoutParams(moveBook, anchorView);
	}
}
