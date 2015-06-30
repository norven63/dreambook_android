package cn.retech.activity.explain;

import java.util.ArrayList;
import java.util.List;

import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.view.ViewPager;
import android.support.v4.view.ViewPager.OnPageChangeListener;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import cn.retech.activity.R;
import cn.retech.adapter.MyFragmentPagerAdapter;

public class ExplainActivity extends FragmentActivity {
	private MyFragmentPagerAdapter viewPaperAdapter = null;

	private ViewPager viewPager;
	private LinearLayout navigateImageLayout;

	@Override
	protected void onCreate(Bundle arg0) {
		super.onCreate(arg0);

		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

		setContentView(R.layout.activity_explain);

		List<Fragment> channelFragments = new ArrayList<Fragment>();
		viewPaperAdapter = new MyFragmentPagerAdapter(getSupportFragmentManager(), channelFragments);

		Bundle bundle3 = new Bundle();
		bundle3.putInt(ExplainFragment.ARGUMENTKEY, R.layout.frgment_explain_change);
		channelFragments.add(Fragment.instantiate(this, ChangeExplainFragment.class.getName(), bundle3));

		Bundle bundle1 = new Bundle();
		bundle1.putInt(ExplainFragment.ARGUMENTKEY, R.layout.frgment_explain_delete);
		channelFragments.add(Fragment.instantiate(this, DeleteExplainFragment.class.getName(), bundle1));

		Bundle bundle2 = new Bundle();
		bundle2.putInt(ExplainFragment.ARGUMENTKEY, R.layout.frgment_explain_create);
		channelFragments.add(Fragment.instantiate(this, CreateExplainFragment.class.getName(), bundle2));

		Bundle bundle4 = new Bundle();
		bundle4.putInt(ExplainFragment.ARGUMENTKEY, R.layout.frgment_explain_finish);
		boolean isStartByOption = getIntent().getBooleanExtra(ExplainFragment.ISSTARTBYOPTION, false);
		bundle4.putBoolean(ExplainFragment.ISSTARTBYOPTION, isStartByOption);
		channelFragments.add(Fragment.instantiate(this, FinishExplainFragment.class.getName(), bundle4));

		viewPager = (ViewPager) findViewById(R.id.explain_viewPager);
		viewPager.setAdapter(viewPaperAdapter);
		viewPager.setOnPageChangeListener(new OnPageChangeListener() {
			@Override
			public void onPageSelected(int position) {
				ExplainFragment currentExplainFragment = (ExplainFragment) viewPaperAdapter.getItem(position);
				currentExplainFragment.startAnimation();

				if (position == 3) {
					navigateImageLayout.setVisibility(View.INVISIBLE);
				} else {
					navigateImageLayout.setVisibility(View.VISIBLE);
					for (int i = 0; i < navigateImageLayout.getChildCount(); i++) {
						ImageView childImageView = (ImageView) navigateImageLayout.getChildAt(i);
						childImageView.setBackgroundResource(R.drawable.explain_navigat_point);
					}

					ImageView childImageViewSelectoed = (ImageView) navigateImageLayout.getChildAt(position);
					childImageViewSelectoed.setBackgroundResource(R.drawable.explain_navigat_point_selected);
				}
			}

			@Override
			public void onPageScrolled(int arg0, float arg1, int arg2) {
			}

			@Override
			public void onPageScrollStateChanged(int arg0) {
			}
		});

		navigateImageLayout = (LinearLayout) findViewById(R.id.navigatorImage_layout);
		ImageView firstChildImageView = (ImageView) navigateImageLayout.getChildAt(0);
		firstChildImageView.setBackgroundResource(R.drawable.explain_navigat_point_selected);

		viewPager.setCurrentItem(0);
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);

		ExplainFragment firstExplainFragment = (ExplainFragment) viewPaperAdapter.getItem(0);
		firstExplainFragment.startAnimation();
	}
}
