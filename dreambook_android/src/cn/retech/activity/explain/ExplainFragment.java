package cn.retech.activity.explain;

import static com.google.common.base.Preconditions.checkArgument;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import android.animation.Animator;
import android.animation.Animator.AnimatorListener;
import android.animation.AnimatorSet;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.ImageView.ScaleType;
import cn.retech.activity.R;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

public abstract class ExplainFragment extends Fragment {
	public ExplainFragment() {
	}

	public interface ICurrentState {
		public void animation();
	}

	public enum StaticViewStateKeyEnum {
		MOVEBOOK_Y,
		//
		MOVEBOOK_X,
		//
		HAND_X,
		//
		HAND_Y,
		//
		TOUCH_POINT_X,
		//
		TOUCH_POINT_Y,
		//
		GRIDVIEW_X,
		//
		GRIDVIEW2_X,
		//
		GRIDVIEW2_Y,
		//
		NATIVEIMAGEVIEW_X,
		//
		DELETEVIEW_X,
		//
		DELETEVIEW_Y;
	}

	public class MyBaseAdapter extends BaseAdapter {
		List<View> views = Lists.newArrayList();
		{
			for (int i = 0; i < 9; i++) {
				ImageView imageView = new ImageView(getActivity());
				imageView.setImageResource(R.drawable.ic_explain_static_book);
				imageView.setScaleType(ScaleType.FIT_XY);
				imageView.setLayoutParams(new AbsListView.LayoutParams(getResources().getInteger(R.integer.moveBook_width), getResources()
						.getInteger(R.integer.moveBook_height)));

				views.add(imageView);
			}
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			return getItem(position);
		}

		@Override
		public long getItemId(int position) {
			return position;
		}

		@Override
		public View getItem(int position) {
			checkArgument(position >= 0 || position < views.size(), "入参不合法: position=" + position + " views.size()=" + views.size());

			return views.get(position);
		}

		@Override
		public int getCount() {
			return views.size();
		}

		@Override
		public boolean isEnabled(int position) {
			return false;
		}
	}

	public static final String ARGUMENTKEY = "contentViewId";
	public static final String ISSTARTBYOPTION = "isStartByOption";

	protected ImageView titleBar;
	protected GridView gridView;
	protected ImageView anchorView;
	protected ImageView touchPoint;
	protected ImageView hand;

	protected LinkedList<ICurrentState> states = Lists.newLinkedList();
	protected ArrayList<AnimatorSet> needCancelAnimatorSets = Lists.newArrayList();
	protected volatile boolean hasAnimator = false;
	protected final long animatorTime = 1500;
	protected float pointX;
	protected float pointY;

	protected Map<StaticViewStateKeyEnum, Number> staticViewStateMap = Maps.newHashMap();

	protected class AnimatorFinishListener implements AnimatorListener {
		@Override
		public void onAnimationStart(Animator animation) {
		}

		@Override
		public void onAnimationEnd(Animator animation) {
			palyNextAnimation();
		}

		@Override
		public void onAnimationCancel(Animator animation) {
			animation.removeAllListeners();
		}

		@Override
		public void onAnimationRepeat(Animator animation) {
		}
	}

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		return inflater.inflate(getArguments().getInt(ARGUMENTKEY), container, false);
	}

	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);

		initStates();

		titleBar = (ImageView) getView().findViewById(R.id.explain_titleBar_imageView);
		touchPoint = (ImageView) getView().findViewById(R.id.touch_point_imageView);
		hand = (ImageView) getView().findViewById(R.id.hand_imageView);

		gridView = (GridView) getView().findViewById(R.id.explain_gridView);
		if (gridView != null) {
			gridView.setAdapter(new MyBaseAdapter());
		}
	}

	public void startAnimation() {
		if (states.size() <= 0) {
			return;
		}

		if (hasAnimator) {
			finallyLayout();

			return;
		}

		hasAnimator = true;

		initiativeLayout();

		palyNextAnimation();
	}

	private void stopAnimation() {
		synchronized (needCancelAnimatorSets) {
			for (AnimatorSet animatorSet : needCancelAnimatorSets) {
				animatorSet.removeAllListeners();
				animatorSet.cancel();
			}
		}
	}

	private void palyNextAnimation() {
		ICurrentState iCurrentState = states.pop();
		iCurrentState.animation();
	}

	protected abstract void initStates();

	protected void initiativeLayout() {
		pointX = anchorView.getX() + anchorView.getWidth() / 2;
		pointY = anchorView.getY() + anchorView.getHeight() / 2 + titleBar.getHeight();

		hand.setX(pointX);
		hand.setY(pointY);
		hand.setVisibility(View.VISIBLE);

		touchPoint.setX(pointX - touchPoint.getWidth() / 2);
		touchPoint.setY(pointY - touchPoint.getHeight() / 2);
		touchPoint.setVisibility(View.VISIBLE);
	}

	protected void finallyLayout() {
		hand.setX((Float) staticViewStateMap.get(StaticViewStateKeyEnum.HAND_X));
		hand.setY((Float) staticViewStateMap.get(StaticViewStateKeyEnum.HAND_Y));
		hand.setVisibility(View.VISIBLE);

		touchPoint.setX((Float) staticViewStateMap.get(StaticViewStateKeyEnum.TOUCH_POINT_X));
		touchPoint.setY((Float) staticViewStateMap.get(StaticViewStateKeyEnum.TOUCH_POINT_Y));
		touchPoint.setVisibility(View.VISIBLE);
	}

	@Override
	public void onPause() {
		super.onPause();

		stopAnimation();
	}
}
