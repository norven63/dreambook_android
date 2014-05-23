package cn.retech.activity.explain;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ImageButton;
import cn.retech.activity.LoginActivity;
import cn.retech.activity.R;
import cn.retech.global_data_cache.GlobalDataCacheForMemorySingleton;

public class FinishExplainFragment extends ExplainFragment {
	public FinishExplainFragment() {
		super();
	}

	private ImageButton gotoShelfButton;

	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);

		gotoShelfButton = (ImageButton) getView().findViewById(R.id.gotoShelf_imageButton);
		gotoShelfButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (!getArguments().getBoolean(ISSTARTBYOPTION)) {
					GlobalDataCacheForMemorySingleton.getInstance.setFirstStartApp(false);

					Intent intent = new Intent();
					intent.setClass(getActivity(), LoginActivity.class);
					FinishExplainFragment.this.startActivity(intent);
				}

				getActivity().finish();
			}
		});
	}

	@Override
	protected void initStates() {
	}
}
