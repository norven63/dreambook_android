package cn.retech.activity;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;
import cn.retech.activity.explain.ExplainActivity;
import cn.retech.custom_control.CustomControlDelegate;
import cn.retech.custom_control.WelcomeCustomControl;
import cn.retech.custom_control.WelcomeCustomControl.WelcomeCustomControlTypeEnum;
import cn.retech.global_data_cache.GlobalDataCacheForMemorySingleton;
import cn.retech.theme_manage.ThemeManagerSingleton;
import cn.retech.toolutils.DebugLog;

import com.umeng.analytics.MobclickAgent;

public class WelcomeActivity extends Activity implements CustomControlDelegate {
	private final String TAG = this.getClass().getSimpleName();
	private WelcomeCustomControl welcomeCustomControl;

	@Override
	public void customControlOnAction(Object contorl, Object actionTypeEnum) {
		if (actionTypeEnum instanceof WelcomeCustomControlTypeEnum) {
			switch ((WelcomeCustomControlTypeEnum) actionTypeEnum) {
			case FINISH:
				Intent intent = new Intent();
				if (GlobalDataCacheForMemorySingleton.getInstance.isFirstStartApp()) {
					intent.setClass(WelcomeActivity.this, ExplainActivity.class);
				} else {
					if (GlobalDataCacheForMemorySingleton.getInstance.isLogged()) {
						ThemeManagerSingleton.getInstance.loadLocalThemeByCompanyID(GlobalDataCacheForMemorySingleton.getInstance.getCompanyid());
						// 已经登录
						intent.setClass(WelcomeActivity.this, BookShelfActivity.class);
					} else {
						intent.setClass(WelcomeActivity.this, LoginActivity.class);
					}
				}

				WelcomeActivity.this.startActivity(intent);
				WelcomeActivity.this.finish();

				break;
			default:
				break;
			}
		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		DebugLog.i(TAG, "onCreate");
		super.onCreate(savedInstanceState);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.activity_welcome_layout);

		welcomeCustomControl = (WelcomeCustomControl) findViewById(R.id.welcome_custon_control);
		welcomeCustomControl.setDelegate(this);
	}

	@Override
	protected void onDestroy() {
		DebugLog.i(TAG, "onDestroy");

		super.onDestroy();
	}

	@Override
	protected void onPause() {
		DebugLog.i(TAG, "onPause");
		super.onPause();

		MobclickAgent.onPause(this);
	}

	@Override
	protected void onRestart() {
		DebugLog.i(TAG, "onRestart");
		super.onRestart();
	}

	@Override
	protected void onResume() {
		DebugLog.i(TAG, "onResume");
		super.onResume();

		MobclickAgent.onResume(this);
	}

	@Override
	protected void onStart() {
		DebugLog.i(TAG, "onStart");
		super.onStart();
	}

	@Override
	protected void onStop() {
		DebugLog.i(TAG, "onStop");
		super.onStop();
	}

}
