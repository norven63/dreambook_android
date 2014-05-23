package cn.retech.activity;

import java.util.List;

import android.animation.Animator;
import android.animation.Animator.AnimatorListener;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;
import cn.retech.custom_control.AnimatorDirector;
import cn.retech.custom_control.AnimatorDirector.AnimatorBuilder;
import cn.retech.custom_control.CircleProgressObservable;
import cn.retech.domainbean_model.book.Book;
import cn.retech.domainbean_model.login.LoginNetRequestBean;
import cn.retech.domainbean_model.login.LoginNetRespondBean;
import cn.retech.global_data_cache.GlobalDataCacheForMemorySingleton;
import cn.retech.global_data_cache.GlobalDataCacheForNeedSaveToFileSystem;
import cn.retech.my_network_engine.IDomainBeanAsyncHttpResponseListener;
import cn.retech.my_network_engine.INetRequestHandle;
import cn.retech.my_network_engine.NetRequestHandleNilObject;
import cn.retech.my_network_engine.SimpleNetworkEngineSingleton;
import cn.retech.my_network_engine.net_error_handle.MyNetRequestErrorBean;
import cn.retech.theme_manage.IThemeDownloadResponseHandler;
import cn.retech.theme_manage.ThemeManagerSingleton;
import cn.retech.toolutils.DebugLog;
import cn.retech.toolutils.FastDoubleClickTest;
import cn.retech.toolutils.ToolsFunctionForThisProgect;

import com.umeng.analytics.MobclickAgent;

public class LoginActivity extends Activity {
  private final String TAG = this.getClass().getSimpleName();
  private INetRequestHandle netRequestHandleForLogin = new NetRequestHandleNilObject();

  private EditText userNameEditText;
  private EditText passwordEditText;
  private Button loginButton;

  private LinearLayout progressLayout;
  private ProgressBar downloadProgressBar;
  private ImageButton downloadErrorButton;
  private TextView downloadMessage;

  private final Handler handler = new Handler();

  private final OnClickListener downlaodErrorProgressBarOnClickListener = new OnClickListener() {
    @Override
    public void onClick(View v) {
      if (FastDoubleClickTest.isFastDoubleClick()) {
        return;
      }

      showDialogForDownload();
    }
  };

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    // 获取标识位，用来设置是否支持屏幕翻转(手机不支持屏幕翻转)
    if (!getResources().getBoolean(R.bool.isLarge)) {
      setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

      // setTheme(R.style.AppTheme);
    }

    setContentView(R.layout.activity_login_layout);

    userNameEditText = (EditText) findViewById(R.id.user_name_editText);
    passwordEditText = (EditText) findViewById(R.id.password_editText);
    loginButton = (Button) findViewById(R.id.login_button);
    progressLayout = (LinearLayout) findViewById(R.id.progressLayout_layout);
    downloadProgressBar = (ProgressBar) findViewById(R.id.login_download_progressBar);
    downloadErrorButton = (ImageButton) findViewById(R.id.login_downloadError_imageButton);
    downloadMessage = (TextView) findViewById(R.id.login_downloadMessage_textView);

    if (DebugLog.logIsOpen) {
      userNameEditText.setText("appletest");
      passwordEditText.setText("appletest");
    }

    loginButton.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        if (checkParametersAndLogin()) {
          userNameEditText.setEnabled(false);
          passwordEditText.setVisibility(View.GONE);
          loginButton.setVisibility(View.GONE);
        }
      }
    });

  }

  private boolean requestLogin(final String userID, final String userPassWord) {
    LoginNetRequestBean netRequestBean = new LoginNetRequestBean.Builder(userID, userPassWord).builder();
    netRequestHandleForLogin = SimpleNetworkEngineSingleton.getInstance.requestDomainBean(netRequestBean, new IDomainBeanAsyncHttpResponseListener() {
      @Override
      public void onFailure(MyNetRequestErrorBean error) {
        // 当登录失败需要将登录框复位
        Toast.makeText(LoginActivity.this, error.getErrorMessage(), Toast.LENGTH_SHORT).show();
        userNameEditText.setEnabled(true);
        passwordEditText.setVisibility(View.VISIBLE);
        loginButton.setVisibility(View.VISIBLE);
      }

      @Override
      public void onSuccess(final Object respondDomainBean) {

        // 登陆成功后 editText需要下移30
        if (null == userNameEditText.getTag(R.id.last_y)) {
          userNameEditText.setTag(R.id.last_y, userNameEditText.getY());
        }
        AnimatorDirector.animate(new AnimatorBuilder().view(userNameEditText).y(userNameEditText.getY() + 30).setDuration(1000));

        ThemeManagerSingleton.getInstance.setContext(LoginActivity.this);
        ThemeManagerSingleton.getInstance.requestThemeWithUserName(userID, userPassWord, new IThemeDownloadResponseHandler() {
          @Override
          public void onMissionComplete() {
            downloadMessage.setText("数据解压完成,界面跳转...");
            // 用户登录成功
            LoginNetRespondBean latestLogonNetRespondBean = (LoginNetRespondBean) respondDomainBean;
            GlobalDataCacheForMemorySingleton.getInstance.noteSignInSuccessfulInfo(latestLogonNetRespondBean, userID, userPassWord);
            // 登录成功,跳转到书架界面
            Intent intent = new Intent();
            intent.setClass(LoginActivity.this, BookShelfActivity.class);
            LoginActivity.this.startActivity(intent);
            LoginActivity.this.finish();
          }

          @Override
          public void onProgressDoInBackground(final long bytesWritten, final long totalSize) {
            if (progressLayout.getVisibility() == View.GONE || progressLayout.getVisibility() == View.INVISIBLE) {
              progressLayout.setVisibility(View.VISIBLE);
              downloadMessage.setVisibility(View.VISIBLE);
            }
            handler.post(new Runnable() {
              @Override
              public void run() {
                downloadProgressBar.setProgress((int) (bytesWritten * 100 / totalSize));
              }
            });
          }

          @Override
          public void onFailure(final MyNetRequestErrorBean error) {
            handler.post(new Runnable() {
              @Override
              public void run() {
                downloadErrorButton.setVisibility(View.VISIBLE);
                downloadErrorButton.setOnClickListener(downlaodErrorProgressBarOnClickListener);

                downloadMessage.setText(getResources().getString(R.string.downloadError));

                downloadProgressBar.setProgressDrawable(getResources().getDrawable(R.drawable.shape_progress_bar_style_error));

                Toast.makeText(LoginActivity.this, error.getErrorMessage(), Toast.LENGTH_SHORT).show();
              }
            });
          }

          @Override
          public void onDownloadSuccess() {
            downloadMessage.setVisibility(View.VISIBLE);
            downloadProgressBar.setVisibility(View.INVISIBLE);
            downloadMessage.setText("数据解压中...");
          }
        });
      }
    });

    return !netRequestHandleForLogin.idle();
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

  @Override
  public boolean onKeyDown(int keyCode, KeyEvent event) {
    if (keyCode == KeyEvent.KEYCODE_BACK) {
      showDialogForQuiteApp();
      return true;
    }
    return super.onKeyDown(keyCode, event);
  }

  private void showDialogForDownload() {
    AlertDialog.Builder builder = new AlertDialog.Builder(LoginActivity.this);
    builder.create();
    builder.setTitle("提示");
    builder.setMessage("是否重新下载?");
    builder.setPositiveButton("确认", new DialogInterface.OnClickListener() {
      @Override
      public void onClick(DialogInterface dialog, int which) {
        downloadErrorButton.setVisibility(View.GONE);

        downloadProgressBar.setProgress(0);
        downloadProgressBar.setProgressDrawable(getResources().getDrawable(R.drawable.shape_progress_bar_style));

        downloadMessage.setText(getResources().getString(R.string.downloading));

        checkParametersAndLogin();
      }
    }).setNegativeButton("取消", new DialogInterface.OnClickListener() {
      @Override
      public void onClick(DialogInterface dialog, int which) {
        dialog.cancel();

        progressLayout.setVisibility(View.GONE);

        downloadErrorButton.setVisibility(View.GONE);

        downloadProgressBar.setProgress(0);
        downloadProgressBar.setProgressDrawable(getResources().getDrawable(R.drawable.shape_progress_bar_style));

        downloadMessage.setVisibility(View.GONE);
        downloadMessage.setText(getResources().getString(R.string.downloading));

        AnimatorDirector.animate(new AnimatorBuilder().view(userNameEditText).y((Float) userNameEditText.getTag(R.id.last_y)).setDuration(1000).setListener(new AnimatorListener() {
          @Override
          public void onAnimationStart(Animator animation) {
          }

          @Override
          public void onAnimationRepeat(Animator animation) {
          }

          @Override
          public void onAnimationEnd(Animator animation) {
            userNameEditText.setEnabled(true);
            passwordEditText.setVisibility(View.VISIBLE);
            loginButton.setVisibility(View.VISIBLE);
          }

          @Override
          public void onAnimationCancel(Animator animation) {
          }
        }));

        downloadErrorButton.setOnClickListener(null);
      }
    });
    builder.show();
  }

  /**
   * 显示 "退出应用程序" 提示框
   */
  private void showDialogForQuiteApp() {
    AlertDialog.Builder builder = new Builder(LoginActivity.this);
    builder.setTitle("提示");
    builder.setMessage("是否退出应用");
    builder.setPositiveButton("确认", new DialogInterface.OnClickListener() {
      @Override
      public void onClick(DialogInterface dialog, int which) {
        dialog.cancel();
        // 先暂停用于异步保存LocalBook的线程
        GlobalDataCacheForNeedSaveToFileSystem.stopSaveLocalBookThread();

        // 这里需要先将所有正在下载的书籍暂停，然后才能退出应用
        List<Book> books = GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList().getShallowCopyBookList();
        for (Book book : books) {
          book.stopDownloadBook();
        }
        CircleProgressObservable.INSTANCE.stop();
        // 停止一个服务
        Intent intent = new Intent(LoginActivity.this, UpdateSoftwareService.class);
        // 采用 startService() 启动的服务, 必须主动调用 stopService()来主动停止
        stopService(intent);
        // 退出应用
        ToolsFunctionForThisProgect.quitApp(LoginActivity.this);
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
  }

  private boolean checkParametersAndLogin() {
    String errorMessage = "";
    do {
      if (TextUtils.isEmpty(userNameEditText.getText())) {
        errorMessage = "用户名不能为空!";
        break;
      }

      if (TextUtils.isEmpty(passwordEditText.getText())) {
        errorMessage = "密码不能为空!";
        break;
      }

      if (!netRequestHandleForLogin.idle()) {
        errorMessage = "不能重复发起登录操作!";
        break;
      }

      if (!requestLogin(userNameEditText.getText().toString().trim(), passwordEditText.getText().toString().trim())) {
        errorMessage = "发起登录请求失败!";
        break;
      }
      return true;
    } while (false);

    Toast.makeText(LoginActivity.this, errorMessage, Toast.LENGTH_SHORT).show();
    return false;
  }
}
