package cn.retech.activity;

import java.io.File;

import android.app.Application;
import android.content.res.Configuration;
import android.util.DisplayMetrics;
import cn.retech.custom_control.CircleProgressObservable;
import cn.retech.global_data_cache.GlobalDataCacheForMemorySingleton;
import cn.retech.global_data_cache.GlobalDataCacheForNeedSaveToFileSystem;
import cn.retech.global_data_cache.LocalCacheDataPathConstant;
import cn.retech.image_loader.MyImageDownloader;
import cn.retech.rich_text_reader.BookReader;
import cn.retech.toolutils.DebugLog;
import cn.retech.toolutils.ToolsFunctionForThisProgect;

import com.nostra13.universalimageloader.cache.disc.impl.UnlimitedDiskCache;
import com.nostra13.universalimageloader.cache.disc.naming.HashCodeFileNameGenerator;
import com.nostra13.universalimageloader.cache.memory.impl.LruMemoryCache;
import com.nostra13.universalimageloader.core.DisplayImageOptions;
import com.nostra13.universalimageloader.core.ImageLoader;
import com.nostra13.universalimageloader.core.ImageLoaderConfiguration;
import com.nostra13.universalimageloader.core.assist.QueueProcessingType;
import com.nostra13.universalimageloader.core.decode.BaseImageDecoder;
import com.nostra13.universalimageloader.utils.StorageUtils;

// 在Android中启用断言的方法： adb shell setprop debug.assert 1
// mac系统配置 adb
// 一、终端中输入 cd ~
// 二、输入 touch .bash_profile 回车
// 三、输入 open -e .bash_profile
// 输入 : export
// PATH=$PATH:/Applications/developer/android-sdk-macosx/tools:/Applications/developer/android-sdk-macosx/platform-tools
/**
 * 自定义 Application
 * 
 * @author hesiming
 * 
 */
public class MyApplication extends Application {
	private final String TAG = this.getClass().getSimpleName();

	// Application 类对外的接口
	private static MyApplication self;

	public static Application getApplication() {
		return self;
	}

	@Override
	public void onCreate() {

		DebugLog.i(TAG, "onCreate");

		super.onCreate();

		// 必须在第一个行的位置
		self = this;

		GlobalDataCacheForMemorySingleton.getInstance.setApplication(self);
		// 创建本地缓存目录
		LocalCacheDataPathConstant.createLocalCacheDirectories();

		// 配置ImageLoader============start============
		/*
		 * 配置ImageLoader
		 */
		File cacheDir = StorageUtils.getCacheDirectory(this);
		ImageLoaderConfiguration config = new ImageLoaderConfiguration.Builder(this).memoryCacheExtraOptions(480, 800) // default
																																																																				// =
																																																																				// device
																																																																				// screen
																																																																				// dimensions
				.diskCacheExtraOptions(480, 800, null).threadPoolSize(3) // default
				.threadPriority(Thread.NORM_PRIORITY - 2) // default
				.tasksProcessingOrder(QueueProcessingType.FIFO) // default
				.denyCacheImageMultipleSizesInMemory().memoryCache(new LruMemoryCache(2 * 1024 * 1024)).memoryCacheSize(2 * 1024 * 1024).memoryCacheSizePercentage(13) // default
				.diskCache(new UnlimitedDiskCache(cacheDir)) // default
				.diskCacheSize(50 * 1024 * 1024).diskCacheFileCount(100).diskCacheFileNameGenerator(new HashCodeFileNameGenerator()) // default
				.imageDownloader(new MyImageDownloader(this)) // default
				.imageDecoder(new BaseImageDecoder(true)) // default
				.defaultDisplayImageOptions(DisplayImageOptions.createSimple()) // default
				.writeDebugLogs().build();

		ImageLoader.getInstance().init(config);
		// 配置ImageLoader============end============

		// 读取本地缓存的重要数据
		GlobalDataCacheForNeedSaveToFileSystem.readAllCacheData();
		// 获取本地的渠道号
		GlobalDataCacheForMemorySingleton.getInstance.setChannelName(ToolsFunctionForThisProgect.getUmengChannel());
		// 获取应用的版本号
		GlobalDataCacheForMemorySingleton.getInstance.setVersionName(ToolsFunctionForThisProgect.getVersionName());

		CircleProgressObservable.INSTANCE.start();

		// 流式阅读环境初始化===========start===========
		int screenWidth = getResources().getDisplayMetrics().widthPixels;
		int screenHeight = getResources().getDisplayMetrics().heightPixels;

		DisplayMetrics dm = new DisplayMetrics();
		dm = getResources().getDisplayMetrics();
		int screenDensityDPI = dm.densityDpi;

		BookReader.getInstance.initProperty(screenWidth, screenHeight, screenDensityDPI);
		// 流式阅读环境初始化===========end=============
	}

	@Override
	public void onTerminate() {
		DebugLog.d(TAG, "onTerminate");
		// 父类方法, 必须调用
		super.onTerminate();
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		DebugLog.i(TAG, "onConfigurationChanged");
		super.onConfigurationChanged(newConfig);

	}

	@Override
	public void onLowMemory() {
		DebugLog.i(TAG, "onLowMemory");
		super.onLowMemory();

	}
}
