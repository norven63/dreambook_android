package cn.retech.activity;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Method;

import android.annotation.SuppressLint;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.support.v4.app.NotificationCompat;
import android.support.v4.app.TaskStackBuilder;
import android.widget.Toast;
import cn.retech.my_network_engine.IFileAsyncHttpResponseListenerOnProgressDoInBackground;
import cn.retech.my_network_engine.SimpleNetworkEngineSingleton;
import cn.retech.my_network_engine.net_error_handle.MyNetRequestErrorBean;
import cn.retech.toolutils.DebugLog;
import cn.retech.toolutils.MD5Util;

/**
 * 启动App后, 就会激活当前服务, 当前服务的功能就是加载一些耗时的资源
 * 
 * @author hesiming
 */
// 如果先采用startService()方法启动服务,然后调用bindService()方法绑定到服务，
// 再调用unbindService()方法解除绑定，最后调用bindService()方法再次绑定到服务，
// 触发的生命周期方法如下：
// onCreate()onStart()onBind()onUnbind()[重载后的方法需返回true]onRebind()
public class UpdateSoftwareService extends Service {
	private final String TAG = this.getClass().getSimpleName();
	private Intent updateIntent;
	private static final int DOWNLOAD_SUCCESS = 0;
	private static final int DOWNLOAD_COMPLETE = 1;
	private static final int DOWNLOAD_FALL = 2;
	// BASE Notification ID
	private int Notification_ID_BASE = 110;
	public static final String TRACK_VIEW_URL = "trackViewUrl";
	public static final String MD5 = "MD5";
	private String md5String = "";

	// onCreate()该方法在服务被创建时调用，该方法只会被调用一次，
	// 无论调用多少次startService()或bindService()方法，服务也只被创建一次。
	@Override
	public void onCreate() {
		super.onCreate();
		DebugLog.i(TAG, "onCreate");
		showCustomizeNotification();

	}

	public int getNotification_ID_BASE() {
		return Notification_ID_BASE;
	}

	// onStart() 只有采用Context.startService()方法启动服务时才会回调该方法。
	// 该方法在服务开始运行时被调用。多次调用startService()方法尽管不会多次创建服务，
	// 但onStart() 方法会被多次调用。
	@Override
	public void onStart(Intent intent, int startId) {
		DebugLog.i(TAG, "onStart");
		updateIntent = intent;
		md5String = updateIntent.getStringExtra(MD5);
		super.onStart(intent, startId);
		downLoadApk();
	}

	// onDestroy()该方法在服务被终止时调用。
	@Override
	public void onDestroy() {
		DebugLog.i(TAG, "onDestroy");
		mNotificationManager.cancel(Notification_ID_BASE);
		super.onDestroy();
	}

	@Override
	public void onLowMemory() {
		DebugLog.i(TAG, "onLowMemory");
		super.onLowMemory();
	}

	@Override
	public void onRebind(Intent intent) {
		DebugLog.i(TAG, "onRebind");
		super.onRebind(intent);
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		DebugLog.i(TAG, "onStartCommand");
		return super.onStartCommand(intent, flags, startId);
	}

	// onBind()只有采用Context.bindService()方法启动服务时才会回调该方法。
	// 该方法在调用者与服务绑定时被调用，当调用者与服务已经绑定，
	// 多次调用Context.bindService()方法并不会导致该方法被多次调用。
	@Override
	public IBinder onBind(Intent arg0) {
		DebugLog.i(TAG, "onBind");
		return null;
	}

	// onUnbind()只有采用Context.bindService()方法启动服务时才会回调该方法。
	// 该方法在调用者与服务解除绑定时被调用。
	@Override
	public boolean onUnbind(Intent intent) {
		DebugLog.i(TAG, "onUnbind");
		return super.onUnbind(intent);
	}

	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	private void downLoadApk() {
		SimpleNetworkEngineSingleton.getInstance.requestFileDownlaod(updateIntent.getStringExtra(TRACK_VIEW_URL), getApplicationContext().getFilesDir() + "/DreamBook_update.apk",
				new IFileAsyncHttpResponseListenerOnProgressDoInBackground() {

					@Override
					public void onSuccess(File file) {
						Message msg = new Message();
						msg.what = DOWNLOAD_SUCCESS;
						myHandler.sendMessage(msg);
					}

					@Override
					public void onFailure(MyNetRequestErrorBean error) {
						Toast.makeText(UpdateSoftwareService.this, error.getErrorMessage(), Toast.LENGTH_SHORT).show();
						Message msg = new Message();
						msg.what = DOWNLOAD_FALL;
						myHandler.sendMessage(msg);
					}

					@Override
					public void onProgressDoInBackground(long bytesWritten, long totalSize) {
						float pro = (float) bytesWritten / totalSize * 100.0f;
						int progress = (int) Math.round(pro + 0.5);
						if (pergressCount < progress && progress <= 100) {
							pergressCount = progress;
							Message msg = new Message();
							msg.arg1 = progress;
							msg.what = DOWNLOAD_COMPLETE;
							myHandler.sendMessage(msg);
						}

					}
				});
	}

	private int pergressCount = 0;
	private NotificationManager mNotificationManager;
	private NotificationCompat.Builder mBuilder;

	// 自定义显示的通知 ，创建RemoteView对象
	private void showCustomizeNotification() {

		mBuilder = new NotificationCompat.Builder(this).setSmallIcon(R.drawable.ic_launcher).setContentTitle("今日书院更新").setContentText("今日书院");
		// 为Activity创建一个明确的Intent
		Intent resultIntent = new Intent(this, BookShelfActivity.class);

		// 栈建造者对象将为启动的Activity制造一个人工的回退栈
		// 这样确保了在按回退键的时候，可以从启动的Activity回退到主屏幕上
		TaskStackBuilder stackBuilder = TaskStackBuilder.create(this);
		// 为Intent添加回退栈，但不是Intent本身
		stackBuilder.addParentStack(BookStoreActivity.class);
		// 添加Intent，这个Intent启动Activity并放置它到栈顶
		stackBuilder.addNextIntent(resultIntent);
		PendingIntent resultPendingIntent = stackBuilder.getPendingIntent(0, PendingIntent.FLAG_UPDATE_CURRENT);
		mBuilder.setContentIntent(resultPendingIntent);
		mBuilder.setAutoCancel(true);
		mNotificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
		// mId 允许你在之后更新这个通知
		mNotificationManager.notify(Notification_ID_BASE, mBuilder.build());
	}

	@SuppressLint("HandlerLeak")
	private Handler myHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			switch (msg.what) {
			case DOWNLOAD_SUCCESS:
				installDownloadApk();
				break;
			case DOWNLOAD_COMPLETE:// 下载中状态
				System.out.println(msg.arg1);
				mBuilder.setProgress(100, msg.arg1, false);
				mBuilder.setContentText("下载中" + msg.arg1 + "%");
				mNotificationManager.notify(Notification_ID_BASE, mBuilder.build());
				break;
			case DOWNLOAD_FALL:// 失败状态
				mBuilder.setContentText("下载失败");
				mNotificationManager.notify(Notification_ID_BASE, mBuilder.build());
				stopService(updateIntent);// 停止service
				break;
			default:
				stopService(updateIntent);
			}
		}
	};

	/** 执行Linux命令，并返回执行结果。 */
	public static String exec(String[] args) {
		String result = "";
		ProcessBuilder processBuilder = new ProcessBuilder(args);
		Process process = null;
		InputStream errIs = null;
		InputStream inIs = null;
		try {
			ByteArrayOutputStream baos = new ByteArrayOutputStream();
			int read = -1;
			process = processBuilder.start();
			errIs = process.getErrorStream();
			while ((read = errIs.read()) != -1) {
				baos.write(read);
			}
			baos.write('\n');
			inIs = process.getInputStream();
			while ((read = inIs.read()) != -1) {
				baos.write(read);
			}
			byte[] data = baos.toByteArray();
			result = new String(data);
		} catch (IOException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				if (errIs != null) {
					errIs.close();
				}
				if (inIs != null) {
					inIs.close();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
			if (process != null) {
				process.destroy();
			}
		}
		return result;
	}

	private void installDownloadApk() {
		if (!checkFileMD5(getApplicationContext().getFilesDir() + "/DreamBook_update.apk")) {
			DebugLog.e(TAG, "文件MD5码错误,无法安装!");
			return;
		}

		// 下载完成点击通知进入安装
		Uri uri = Uri.fromFile(new File(getApplicationContext().getFilesDir() + "/DreamBook_update.apk"));
		Intent installIntent = new Intent(Intent.ACTION_VIEW);
		installIntent.setDataAndType(uri, "application/vnd.android.package-archive");
		String[] args1 = { "chmod", "705", getApplicationContext().getFilesDir().toString() };
		exec(args1);
		// [文件604:-rw----r--]
		String[] args2 = { "chmod", "604", getApplicationContext().getFilesDir() + "/DreamBook_update.apk" };
		exec(args2);
		// 这里必须要加Context中有一个startActivity方法，Activity继承自Context，
		// 重载了startActivity方法。如果使用Activity的startActivity方法，不会有任何限制，
		// 而如果使用Context的startActivity方法的话，就需要开启一个新的task，遇到上面那个异常的，都是因为使用了Context的startActivity方法。解决办法是，加一个flag。
		installIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		startActivity(installIntent);
		stopService(updateIntent);// 停止service
	}

	private boolean checkFileMD5(String filename) {
		String fileMd5String = MD5Util.md5sum(filename);
		// 这里使用equalsIgnoreCase是因为使用MD5工具类转出来的MD5码是大写
		// 如果使用equals则会区分大小写,equalsIgnoreCase是不区分大小写的
		if (md5String.equalsIgnoreCase(fileMd5String)) {
			return true;
		}
		return false;
	}

	/**
	 * Collapse status panel自动收起下拉通知栏 此方法无效
	 * 
	 * @param context
	 *            the context used to fetch status bar manager
	 */
	public static void collapseStatusBar(Context context) {
		try {
			Object statusBarManager = context.getSystemService("statusbar");
			Method collapse;

			if (Build.VERSION.SDK_INT <= 16) {
				collapse = statusBarManager.getClass().getMethod("collapse");
			} else {
				collapse = statusBarManager.getClass().getMethod("collapsePanels");
			}
			collapse.invoke(statusBarManager);
		} catch (Exception localException) {
			localException.printStackTrace();
		}
	}
}
