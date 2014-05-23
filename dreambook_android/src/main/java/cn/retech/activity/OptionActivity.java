package cn.retech.activity;

import java.io.File;
import java.util.List;

import android.animation.Animator;
import android.animation.Animator.AnimatorListener;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;
import cn.retech.activity.explain.ExplainActivity;
import cn.retech.custom_control.AnimatorDirector;
import cn.retech.custom_control.AnimatorDirector.AnimatorBuilder;
import cn.retech.domainbean_model.login.LoginNetRespondBean;
import cn.retech.domainbean_model.update.CheckSoftwareUpdateRequestBean;
import cn.retech.domainbean_model.update.CheckSoftwareUpdateRespondBean;
import cn.retech.global_data_cache.GlobalDataCacheForMemorySingleton;
import cn.retech.global_data_cache.LocalCacheDataPathConstant;
import cn.retech.my_network_engine.IDomainBeanAsyncHttpResponseListener;
import cn.retech.my_network_engine.SimpleNetworkEngineSingleton;
import cn.retech.my_network_engine.net_error_handle.MyNetRequestErrorBean;
import cn.retech.theme_manage.ThemeBean;
import cn.retech.theme_manage.ThemeManagerSingleton;
import cn.retech.theme_manage.ThemeModel.ThemeBuilder;
import cn.retech.toolutils.FastDoubleClickTest;
import cn.retech.toolutils.MD5Util;
import cn.retech.toolutils.SimpleFileTools;
import cn.retech.toolutils.SimpleProgressDialog;
import cn.retech.toolutils.ToolsFunctionForThisProgect;

public class OptionActivity extends Activity {

	private RelativeLayout titleBarLayout;
	private ImageView logoBackground;
	private ImageView logoIcon;
	private TextView logoCompanyName;
	private TextView userNickName;
	private Button backButton;

	private TextView titleText;
	private LinearLayout checkUpdateLayout;
	private LinearLayout clearCachLayout;
	private TextView clearCachTextView;
	private LinearLayout helpLinearLayout;
	private LinearLayout aboutLinearLayout;
	private TextView aboutButtonVersionText;
	private Button quitButton;
	private RelativeLayout aboutPageLayout;
	private TextView aboutPageVersionText;

	private final OnClickListener finishActivityClickListener = new OnClickListener() {
		@Override
		public void onClick(View v) {
			if (FastDoubleClickTest.isFastDoubleClick()) {
				return;
			}

			Intent intent = new Intent();
			intent.setClass(OptionActivity.this, BookShelfActivity.class);
			startActivity(intent);
		}
	};

	private final OnClickListener quitAboutContentClickListener = new OnClickListener() {
		@Override
		public void onClick(View v) {
			if (FastDoubleClickTest.isFastDoubleClick()) {
				return;
			}

			quitAboutContent();
		}
	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		// 获取标识位，用来设置是否需要全屏显示设置页面以及禁止屏幕翻转(手机需要)
		if (!getResources().getBoolean(R.bool.isLarge)) {
			setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
			setTheme(android.R.style.Theme_NoTitleBar_Fullscreen);
		}

		setContentView(R.layout.activity_option);

		titleBarLayout = (RelativeLayout) findViewById(R.id.option_titleBar_layout);
		titleText = (TextView) findViewById(R.id.optionTitle_TextView);
		logoBackground = (ImageView) findViewById(R.id.logoBackground_imageView);
		logoIcon = (ImageView) findViewById(R.id.logoIcon_imageView);
		aboutPageLayout = (RelativeLayout) findViewById(R.id.aboutPage_layout);

		LoginNetRespondBean logonNetRespondBean = GlobalDataCacheForMemorySingleton.getInstance.getLatestLogonNetRespondBean();

		logoCompanyName = (TextView) findViewById(R.id.logoCompanyName_textView);
		logoCompanyName.setText(logonNetRespondBean.getCompanyName());

		userNickName = (TextView) findViewById(R.id.userNickName_textView);
		userNickName.setText(logonNetRespondBean.getNameKana());

		backButton = (Button) findViewById(R.id.back_Button);
		backButton.setOnClickListener(finishActivityClickListener);

		checkUpdateLayout = (LinearLayout) findViewById(R.id.checkUpdate_layout);
		checkUpdateLayout.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (FastDoubleClickTest.isFastDoubleClick()) {
					return;
				}

				checkSoftwareUpdate();
			}
		});

		final List<File> toBeClearedFiles = LocalCacheDataPathConstant.directoriesCanBeClearByTheUser();
		clearCachLayout = (LinearLayout) findViewById(R.id.clearCach_layout);
		clearCachLayout.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (FastDoubleClickTest.isFastDoubleClick()) {
					return;
				}

				for (File file : toBeClearedFiles) {
					SimpleFileTools.delAllFile(file.getPath());
				}

				clearCachTextView.setText("清除缓存 (0.0KB) ");
			}
		});

		long cacheTotalSize = 0;
		clearCachTextView = (TextView) findViewById(R.id.clearCach_textView);
		for (File file : toBeClearedFiles) {
			cacheTotalSize += ToolsFunctionForThisProgect.getDirSize(file);
		}
		clearCachTextView.setText("清除缓存 (" + ToolsFunctionForThisProgect.bytesToKbOrMb(cacheTotalSize) + ")");

		helpLinearLayout = (LinearLayout) findViewById(R.id.help_layout);
		helpLinearLayout.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (FastDoubleClickTest.isFastDoubleClick()) {
					return;
				}

				Intent intent = new Intent();
				intent.setClass(OptionActivity.this, ExplainActivity.class);
				intent.putExtra("isStartByOption", true);
				startActivity(intent);
			}
		});

		aboutLinearLayout = (LinearLayout) findViewById(R.id.about_layout);
		aboutLinearLayout.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (FastDoubleClickTest.isFastDoubleClick()) {
					return;
				}

				aboutPageLayout.setVisibility(View.VISIBLE);
				aboutPageLayout.setAlpha(0f);
				backButton.setClickable(false);

				AnimatorDirector.animate(new AnimatorBuilder().view(aboutPageLayout).alpha(1.0f).setDuration(900).setListener(new AnimatorListener() {
					@Override
					public void onAnimationStart(Animator animation) {
					}

					@Override
					public void onAnimationRepeat(Animator animation) {
					}

					@Override
					public void onAnimationEnd(Animator animation) {
						titleText.setText("关于");
						backButton.setText("退回");
						backButton.setOnClickListener(quitAboutContentClickListener);
					}

					@Override
					public void onAnimationCancel(Animator animation) {
					}
				}));
			}
		});

		quitButton = (Button) findViewById(R.id.quit_button);
		quitButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (FastDoubleClickTest.isFastDoubleClick()) {
					return;
				}

				showDialogForLogout();
			}
		});

		String versionString = GlobalDataCacheForMemorySingleton.getInstance.getVersionName();

		aboutPageVersionText = (TextView) findViewById(R.id.aboutPageVersion_textView);
		StringBuffer versionMessage = new StringBuffer();
		if (getResources().getBoolean(R.bool.isLarge)) {
			versionMessage.append("pad版    ");
		} else {
			versionMessage.append("手机版   ");
		}
		versionMessage.append(versionString + "版本");
		aboutPageVersionText.setText(versionMessage.toString());

		aboutButtonVersionText = (TextView) findViewById(R.id.aboutButtonVersion_textView);
		aboutButtonVersionText.setText("v " + versionString);
		/*
		 * 皮肤设置
		 */
		ThemeBean themeBean = ThemeManagerSingleton.getInstance.getNewThemeModel().getThemeBean();

		titleBarLayout.setBackgroundColor(Color.parseColor(themeBean.getMainColor()));
		titleText.setTextColor(Color.parseColor(themeBean.getMainTextColor()));
		logoBackground.setImageDrawable(ThemeBuilder.getBitmapAccordingScreenDensity(this, themeBean.getLogoBackgroundInSetting()));
		logoIcon.setImageDrawable(ThemeBuilder.getBitmapAccordingScreenDensity(this, themeBean.getLogoInSetting()));
		logoCompanyName.setTextColor(Color.parseColor(themeBean.getLogoTextColorInSetting()));
		userNickName.setTextColor(Color.parseColor(themeBean.getLogoTextColorInSetting()));

		final int backButtonTextDefaultColor = Color.parseColor(themeBean.getMainTextColor());
		final int backButtonTextProssedColor = Color.parseColor(themeBean.getMainTextColorHighlighted());
		backButton.setTextColor(backButtonTextDefaultColor);
		backButton.setOnTouchListener(new OnTouchListener() {
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					backButton.setTextColor(backButtonTextProssedColor);
					break;
				case MotionEvent.ACTION_UP:
					backButton.setTextColor(backButtonTextDefaultColor);
					break;
				default:
					break;
				}

				return false;
			}
		});

		final int quitButtonProssedColor = Color.parseColor(themeBean.getLogoutButtonBackgroundColorHighlight());
		final int quitButtonDefaultColor = Color.parseColor(themeBean.getLogoutButtonBackgroundColorDefault());
		quitButton.setBackgroundColor(quitButtonDefaultColor);
		quitButton.setOnTouchListener(new OnTouchListener() {
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					quitButton.setBackgroundColor(quitButtonProssedColor);
					break;
				case MotionEvent.ACTION_UP:
					quitButton.setBackgroundColor(quitButtonDefaultColor);
					break;
				default:
					break;
				}

				return false;
			}
		});
	}

	private void showDialogForLogout() {
		AlertDialog.Builder builder = new AlertDialog.Builder(OptionActivity.this);
		builder.create();
		builder.setTitle("提示");
		builder.setMessage("是否退出当前企业账号?");
		builder.setPositiveButton("确认", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				GlobalDataCacheForMemorySingleton.getInstance.noteSignOutSuccessfulInfo();

				Intent intent = new Intent();
				intent.setClass(OptionActivity.this, LoginActivity.class);
				startActivity(intent);
				finish();
			}
		}).setNegativeButton("取消", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.cancel();
			}
		});
		builder.show();
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK) {
			if (aboutPageLayout.getVisibility() == View.VISIBLE) {
				quitAboutContent();
			} else {
				Intent intent = new Intent();
				intent.setClass(this, BookShelfActivity.class);
				startActivity(intent);
			}

			return true;
		}

		return super.onKeyDown(keyCode, event);
	}

	private void quitAboutContent() {
		titleText.setText("设置");
		backButton.setText("取消");
		backButton.setOnClickListener(finishActivityClickListener);
		AnimatorDirector.animate(new AnimatorBuilder().view(aboutPageLayout).alpha(0.0f).setDuration(700).setListener(new AnimatorListener() {
			@Override
			public void onAnimationStart(Animator animation) {
			}

			@Override
			public void onAnimationRepeat(Animator animation) {
			}

			@Override
			public void onAnimationEnd(Animator animation) {
				aboutPageLayout.setVisibility(View.GONE);
			}

			@Override
			public void onAnimationCancel(Animator animation) {
			}
		}));
	}

	private void checkSoftwareUpdate() {
		if (ToolsFunctionForThisProgect.isServiceRunning(OptionActivity.this, UpdateSoftwareService.class.getName().toString())) {
			return;
		}
		CheckSoftwareUpdateRequestBean netRequestDomainBean = new CheckSoftwareUpdateRequestBean();
		SimpleProgressDialog.show(OptionActivity.this, null);
		SimpleNetworkEngineSingleton.getInstance.requestDomainBean(netRequestDomainBean, new IDomainBeanAsyncHttpResponseListener() {

			@Override
			public void onSuccess(Object respondDomainBean) {
				SimpleProgressDialog.dismiss(OptionActivity.this);
				if (respondDomainBean != null) {
					final CheckSoftwareUpdateRespondBean respondBean = (CheckSoftwareUpdateRespondBean) respondDomainBean;
					do {
						// 判断是否升级
						if (!respondBean.isUpdate()) {
							break;
						}
						// 判断版本是否更新
						if (respondBean.getVersion().equals(GlobalDataCacheForMemorySingleton.getInstance.getVersionName())) {
							// 创建对话框
							AlertDialog.Builder newBuilder = new Builder(OptionActivity.this);
							newBuilder.setTitle("当前已经是最新版本");
							newBuilder.setNegativeButton("确认", new DialogInterface.OnClickListener() {
								@Override
								public void onClick(DialogInterface dialog, int which) {
									dialog.cancel();
								}
							});
							AlertDialog newDialog = newBuilder.create();
							newDialog.getWindow().setType((WindowManager.LayoutParams.TYPE_SYSTEM_ALERT));
							newDialog.show();
							break;
						}
						// 表示需要验证
						final File file = new File(getApplicationContext().getFilesDir() + "/DreamBook_update.apk");
						if (!file.exists()) {
							// 创建对话框
							AlertDialog.Builder builder = new Builder(OptionActivity.this);
							builder.setTitle("版本升级提示");
							builder.setMessage("服务器有新版本是否升级版本");
							builder.setPositiveButton("确认", new DialogInterface.OnClickListener() {
								@Override
								public void onClick(DialogInterface dialog, int which) {
									dialog.cancel();
									// 启动一个服务, 用于预加载相关数据
									Intent intent = new Intent(OptionActivity.this, UpdateSoftwareService.class);
									intent.putExtra(UpdateSoftwareService.TRACK_VIEW_URL, respondBean.getTrackViewUrl());
									intent.putExtra(UpdateSoftwareService.MD5, respondBean.getmD5String());

									// 采用 startService() 启动的服务, 必须主动调用 stopService()
									// 来主动停止
									startService(intent);
								}
							});
							builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
								@Override
								public void onClick(DialogInterface dialog, int which) {
									dialog.cancel();
								}
							});
							AlertDialog dialog = builder.create();
							dialog.show();
						} else {
							String fileMd5String = MD5Util.md5sum(getApplicationContext().getFilesDir() + "/DreamBook_update.apk");
							// 这里使用equalsIgnoreCase是因为使用MD5工具类转出来的MD5码是大写
							// 如果使用equals则会区分大小写,equalsIgnoreCase是不区分大小写的
							if (!respondBean.getmD5String().equalsIgnoreCase(fileMd5String)) {
								// 创建对话框
								AlertDialog.Builder builder = new Builder(OptionActivity.this);
								builder.setTitle("文件MD5码校验错误");
								builder.setMessage("请点击确定重新下载文件");
								builder.setPositiveButton("确认", new DialogInterface.OnClickListener() {
									@Override
									public void onClick(DialogInterface dialog, int which) {
										dialog.cancel();
										// 启动一个服务, 用于预加载相关数据
										Intent intent = new Intent(OptionActivity.this, UpdateSoftwareService.class);
										intent.putExtra(UpdateSoftwareService.TRACK_VIEW_URL, respondBean.getTrackViewUrl());
										intent.putExtra(UpdateSoftwareService.MD5, respondBean.getmD5String());

										// 采用 startService() 启动的服务, 必须主动调用 stopService()
										// 来主动停止
										startService(intent);
									}
								});
								builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
									@Override
									public void onClick(DialogInterface dialog, int which) {
										dialog.cancel();
									}
								});
								AlertDialog dialog = builder.create();
								dialog.getWindow().setType((WindowManager.LayoutParams.TYPE_SYSTEM_ALERT));
								dialog.show();
								return;
							} else {
								// 创建对话框
								AlertDialog.Builder newBuilder = new Builder(OptionActivity.this);
								newBuilder.setTitle("本地有新版本");
								newBuilder.setMessage("是否安装新版本");
								newBuilder.setPositiveButton("确认", new DialogInterface.OnClickListener() {
									@Override
									public void onClick(DialogInterface dialog, int which) {
										// 下载完成点击通知进入安装
										Uri uri = Uri.fromFile(new File(getApplicationContext().getFilesDir() + "/DreamBook_update.apk"));
										Intent installIntent = new Intent(Intent.ACTION_VIEW);
										installIntent.setDataAndType(uri, "application/vnd.android.package-archive");
										String[] args1 = { "chmod", "705", getApplicationContext().getFilesDir().toString() };
										UpdateSoftwareService.exec(args1);
										// [文件604:-rw----r--]
										String[] args2 = { "chmod", "604", getApplicationContext().getFilesDir() + "/DreamBook_update.apk" };
										UpdateSoftwareService.exec(args2);
										// 这里必须要加Context中有一个startActivity方法，Activity继承自Context，
										// 重载了startActivity方法。如果使用Activity的startActivity方法，不会有任何限制，
										// 而如果使用Context的startActivity方法的话，就需要开启一个新的task，遇到上面那个异常的，都是因为使用了Context的startActivity方法。解决办法是，加一个flag。
										installIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
										startActivity(installIntent);
									}
								});
								newBuilder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
									@Override
									public void onClick(DialogInterface dialog, int which) {
										dialog.cancel();
									}
								});
								AlertDialog newDialog = newBuilder.create();
								newDialog.getWindow().setType((WindowManager.LayoutParams.TYPE_SYSTEM_ALERT));
								newDialog.show();
							}
						}

					} while (false);

				}
			}

			@Override
			public void onFailure(MyNetRequestErrorBean error) {
				SimpleProgressDialog.dismiss(OptionActivity.this);
				Toast.makeText(OptionActivity.this, error.getErrorMessage(), Toast.LENGTH_SHORT).show();
			}
		});
	}
}
