package cn.retech.activity.explain;

import android.os.Bundle;
import android.view.View;
import android.widget.GridView;
import android.widget.ImageView;
import cn.retech.activity.R;
import cn.retech.custom_control.AnimatorDirector;
import cn.retech.custom_control.AnimatorDirector.AnimatorBuilder;

public class ChangeExplainFragment extends ExplainFragment {
	public ChangeExplainFragment() {
		super();
	}

	private ImageView nativeImageView;
	private GridView gridView2;

	private final ICurrentState firstState = new ICurrentState() {
		@Override
		public void animation() {
			float gridViewX = gridView.getX();

			staticViewStateMap.put(StaticViewStateKeyEnum.HAND_X, touchPoint.getWidth() / 2 + 0f);
			staticViewStateMap.put(StaticViewStateKeyEnum.HAND_Y, hand.getY());
			AnimatorDirector.animate(new AnimatorBuilder().view(hand).x(touchPoint.getWidth() / 2).setDuration(animatorTime));

			staticViewStateMap.put(StaticViewStateKeyEnum.TOUCH_POINT_X, 0f);
			staticViewStateMap.put(StaticViewStateKeyEnum.TOUCH_POINT_Y, touchPoint.getY());
			AnimatorDirector.animate(new AnimatorBuilder().view(touchPoint).x(0f).setDuration(animatorTime));

			staticViewStateMap.put(StaticViewStateKeyEnum.GRIDVIEW2_X, gridViewX);
			staticViewStateMap.put(StaticViewStateKeyEnum.GRIDVIEW2_Y, gridView2.getY());
			AnimatorDirector.animate(new AnimatorBuilder().view(gridView2).x(gridViewX).setDuration(animatorTime));

			staticViewStateMap.put(StaticViewStateKeyEnum.GRIDVIEW_X, gridView.getX() - gridView.getWidth());
			AnimatorDirector.animate(new AnimatorBuilder().view(gridView).x(gridView.getX() - gridView.getWidth()).setDuration(animatorTime));

			staticViewStateMap.put(StaticViewStateKeyEnum.NATIVEIMAGEVIEW_X, nativeImageView.getX() + nativeImageView.getWidth());
			needCancelAnimatorSets.add(AnimatorDirector.animate(new AnimatorBuilder().view(nativeImageView)
					.x(nativeImageView.getX() + nativeImageView.getWidth()).setDuration(animatorTime).setListener(new AnimatorFinishListener())));
		}
	};

	private final ICurrentState secondState = new ICurrentState() {
		@Override
		public void animation() {
			staticViewStateMap.put(StaticViewStateKeyEnum.HAND_X, hand.getX() + 15f);
			staticViewStateMap.put(StaticViewStateKeyEnum.HAND_Y, hand.getY() + 15f);
			hand.setX(hand.getX() + 15);
			hand.setY(hand.getY() + 15);
			touchPoint.setVisibility(View.INVISIBLE);
		}
	};

	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);

		nativeImageView = (ImageView) getView().findViewById(R.id.navigatorImage_imageView);

		gridView2 = (GridView) getView().findViewById(R.id.explain_gridView2);
		gridView2.setAdapter(gridView.getAdapter());
	}

	@Override
	protected void initStates() {
		states.addFirst(secondState);
		states.addFirst(firstState);
	}

	@Override
	public void startAnimation() {
		anchorView = (ImageView) gridView.getAdapter().getItem(5);

		for (int i = 0; i < gridView.getAdapter().getCount(); i++) {
			ImageView imageView = (ImageView) gridView.getAdapter().getItem(i);
			imageView.setImageResource(R.drawable.ic_explain_dynamic_book);
		}

		super.startAnimation();
	}

	@Override
	protected void initiativeLayout() {
		super.initiativeLayout();

		gridView2.setX(gridView.getWidth());
		gridView2.setY(gridView.getY());
		gridView2.setVisibility(View.VISIBLE);
	}

	@Override
	protected void finallyLayout() {
		super.finallyLayout();

		touchPoint.setVisibility(View.INVISIBLE);

		gridView2.setX((Float) staticViewStateMap.get(StaticViewStateKeyEnum.GRIDVIEW2_X));
		gridView2.setY((Float) staticViewStateMap.get(StaticViewStateKeyEnum.GRIDVIEW2_Y));
		gridView2.setVisibility(View.VISIBLE);

		gridView.setX((Float) staticViewStateMap.get(StaticViewStateKeyEnum.GRIDVIEW_X));

		nativeImageView.setX((Float) staticViewStateMap.get(StaticViewStateKeyEnum.NATIVEIMAGEVIEW_X));
	}
}
