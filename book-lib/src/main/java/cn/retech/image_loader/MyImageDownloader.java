package cn.retech.image_loader;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.SynchronousQueue;

import org.apache.http.Header;

import android.content.Context;
import cn.retech.my_network_engine.http_engine.HttpEngineFactoryMethodSingleton;
import cn.retech.toolutils.DebugLog;

import com.loopj.android.http.AsyncHttpResponseHandler;
import com.nostra13.universalimageloader.core.download.BaseImageDownloader;

/**
 * 
 * @author skyduck
 * 
 */
public class MyImageDownloader extends BaseImageDownloader {
  private final String TAG = this.getClass().getSimpleName();

  public MyImageDownloader(Context context) {
    super(context);
  }

  @Override
  protected InputStream getStreamFromNetwork(String imageUri, Object extra) throws IOException {

    final BlockingQueue<InputStream> blockingQueue = new SynchronousQueue<InputStream>();

    final Thread currentThreadOfOutSide = Thread.currentThread();
    HttpEngineFactoryMethodSingleton.getInstance.getSharedAsyncHttpClient().get(imageUri, new AsyncHttpResponseHandler() {
      @Override
      public void onSuccess(int statusCode, Header[] headers, byte[] responseBody) {
        try {
          blockingQueue.put(new ByteArrayInputStream(responseBody));
        } catch (InterruptedException e) {
          DebugLog.e(TAG, "请求图片成功, 回送图片数据失败-->" + e.getLocalizedMessage());
          e.printStackTrace();
          currentThreadOfOutSide.interrupt();
        }
      }

      @Override
      public void onFailure(int statusCode, Header[] headers, byte[] responseBody, Throwable error) {
        DebugLog.e(TAG, "请求图片失败-->" + error.getLocalizedMessage());
        error.printStackTrace();
        currentThreadOfOutSide.interrupt();
      }
    });

    try {
      return blockingQueue.take();
    } catch (InterruptedException e) {
      e.printStackTrace();
    }

    return null;
  }
}