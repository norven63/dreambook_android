package cn.retech.my_network_engine;

import java.io.File;
import java.util.HashMap;
import java.util.Map;

import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;
import cn.retech.domainbean_model.login.LoginNetRespondBean;
import cn.retech.my_network_engine.domainbean_tools.DomainBeanAbstractFactoryCacheSingleton;
import cn.retech.my_network_engine.domainbean_tools.IDomainBeanAbstractFactory;
import cn.retech.my_network_engine.domainbean_tools.IParseDomainBeanToDataDictionary;
import cn.retech.my_network_engine.engine_helper.EngineHelperSingleton;
import cn.retech.my_network_engine.http_engine.HttpEngineFactoryMethodSingleton;
import cn.retech.my_network_engine.http_engine.IDomainBeanRequestAsyncHttpResponseListener;
import cn.retech.my_network_engine.http_engine.IFileRequestAsyncHttpResponseListener;
import cn.retech.my_network_engine.http_engine.INetRequestIsCancelled;
import cn.retech.my_network_engine.net_error_handle.MyNetErrorCodeEnum;
import cn.retech.my_network_engine.net_error_handle.MyNetRequestErrorBean;
import cn.retech.toolutils.DebugLog;

import com.google.common.collect.Maps;

/**
 * 我们封装的网络引擎
 * 提供以下功能 :
 * 1.提供 requestDomainBean : 用于请求业务Bean
 * 2.提供 requestFileDownlaod : 用于下载文件
 * 
 * @author skyduck
 * 
 */
public enum SimpleNetworkEngineSingleton {
  getInstance;

  private final String TAG = this.getClass().getSimpleName();

  private Handler handler = new Handler(Looper.getMainLooper());

  /**
   * 发起一个业务接口的网络请求(其实就是从服务器同步数据Bean到客户端, 数据Bean将采用常用的数据交换协议进行承载, 如JSON/XML)
   * 
   * @param netRequestDomainBean
   *          网络请求业务Bean
   * @param domainBeanAsyncHttpResponseListener
   *          异步网络响应监听(通过这个监听返回的代码已经处于UI线程了)
   * @return RequestHandle (可以通过这个抽屉来查看本次http请求的状态, 或者取消本次http请求)
   */
  public INetRequestHandle requestDomainBean(final Object netRequestDomainBean, final IDomainBeanAsyncHttpResponseListener domainBeanAsyncHttpResponseListener) {

    DebugLog.i(TAG, " ");
    DebugLog.i(TAG, " ");
    DebugLog.i(TAG, " ");
    DebugLog.i(TAG, "<<<<<<<<<<     发起一个DomainBean网络请求, 参数检验中....     >>>>>>>>>>");
    DebugLog.i(TAG, " ");

    INetRequestHandle requestHandle = new NetRequestHandleNilObject();

    try {
      // effective for java 38 检查参数有效性, 对于共有的方法,
      // 要使用异常机制来通知调用方发生了入参错误.
      if (netRequestDomainBean == null) {
        throw new NullPointerException("入参 netRequestDomainBean 为空.");
      }

      /**
       * 将 "网络请求业务Bean" 的 完整class name 作为和这个业务Bean对应的"业务接口" 绑定的所有相关的处理算法的唯一识别Key
       */
      final String abstractFactoryMappingKey = netRequestDomainBean.getClass().getName();
      DebugLog.i(TAG, "abstractFactoryMappingKey--> " + abstractFactoryMappingKey);

      // 这里的设计使用了 "抽象工厂" 设计模式
      // 获取当前 "网络请求业务Bean对应的抽象工厂对象"
      final IDomainBeanAbstractFactory domainBeanAbstractFactoryObject = DomainBeanAbstractFactoryCacheSingleton.getInstance.getDomainBeanAbstractFactoryObjectByKey(abstractFactoryMappingKey);
      if (domainBeanAbstractFactoryObject == null || !(domainBeanAbstractFactoryObject instanceof IDomainBeanAbstractFactory)) {
        throw new NullPointerException("必须实现 IDomainBeanAbstractFactory 接口");
      }

      // 请求URL
      final String specialPath = domainBeanAbstractFactoryObject.getSpecialPath(netRequestDomainBean);
      final String fullUrlString = EngineHelperSingleton.getInstance.getSpliceFullUrlByDomainBeanSpecialPathStrategyObject().getFullUrlByDomainBeanSpecialPath(specialPath);
      DebugLog.i(TAG, "specialPath--> " + specialPath);
      DebugLog.i(TAG, "fullUrlString--> " + fullUrlString);

      // 要发送到服务器的数据
      Map<String, String> dataDictionary = Maps.newHashMap();
      // TODO : 添加公共参数
      //dataDictionary.putAll(EngineHelperSingleton.getInstance.getDomainBeanRequestPublicParameterStrategyObject().getPublicParameter());
      // TODO : 添加具体接口的具体参数
      final IParseDomainBeanToDataDictionary parseDomainBeanToDataDictionary = domainBeanAbstractFactoryObject.getParseDomainBeanToDDStrategyObject();
      if (parseDomainBeanToDataDictionary != null) {
        dataDictionary.putAll(parseDomainBeanToDataDictionary.parseDomainBeanToDataDictionary(netRequestDomainBean));
        DebugLog.i(TAG, "dataDictionary--> " + dataDictionary);
      }

      // 获取http引擎
      requestHandle = HttpEngineFactoryMethodSingleton.getInstance.requestDomainBean(fullUrlString, dataDictionary, new IDomainBeanRequestAsyncHttpResponseListener() {

        @Override
        public void onSuccess(final INetRequestIsCancelled netRequestIsCancelled, final Object response) {
          DebugLog.i(TAG, "<<<<<<<<<<     发起的 DomainBean [" + abstractFactoryMappingKey + "] 网络层请求成功    >>>>>>>>>>");

          final MyNetRequestErrorBean error = new MyNetRequestErrorBean();

          do {

            if (netRequestIsCancelled.isCancelled()) {
              break;
            }

            // ------------------------------------- >>>
            // 将具体网络引擎层返回的 "原始未加工数据byte[]" 解包成 "可识别数据字符串(一般是utf-8)".
            // 这里要考虑网络传回的原生数据有加密的情况, 比如MD5加密的数据, 那么在这里先解密成可识别的字符串
            final Object netUnpackedData = EngineHelperSingleton.getInstance.getNetRespondEntityDataUnpackStrategyObject().unpackNetRespondRawEntityData(response);
            if (netUnpackedData == null) {
              DebugLog.e(TAG, "解析服务器端返回的实体数据失败.");
              error.setErrorCodeEnum(MyNetErrorCodeEnum.kNetErrorCodeEnum_Server_UnpackedResponseDataFailed);
              error.setErrorMessage("解析服务器端返回的实体数据失败.");
              break;
            }
            // ------------------------------------- >>>

            // ------------------------------------- >>>
            // 检查服务器返回的数据是否有效, 如果无效, 要获取服务器返回的错误码和错误描述信息
            // (比如说某次网络请求成功了, 但是服务器那边没有有效的数据给客户端,
            // 所以服务器会返回错误码和描述信息告知客户端访问结果)
            error.reinitialize(EngineHelperSingleton.getInstance.getServerRespondDataTestStrategyObject().testServerRespondDataError(netUnpackedData));
            if (error.getErrorCodeEnum() != MyNetErrorCodeEnum.kNetErrorCodeEnum_Success) {
              // 如果服务器没有有效的数据到客户端, 那么就不需要创建 "网络响应业务Bean"
              DebugLog.e(TAG, "服务器端告知客户端, 本次网络访问未获取到有效数据(具体情况, 可以查看服务器端返回的错误代码和错误信息)");
              DebugLog.e(TAG, error.toString());
              break;
            }
            // ------------------------------------- >>>

            // ------------------------------------- >>>
            // 将 "已经解包的可识别数据字符串" 解析成 "具体的业务响应数据Bean"
            // note : 将服务器返回的数据字符串(已经解密, 解码完成了), 解析成对应的 "网络响应业务Bean"
            try {
              final Object respondDomainBean = domainBeanAbstractFactoryObject.getParseNetRespondDataToDomainBeanStrategyObject().parseNetRespondDataToDomainBean(netUnpackedData);
              DebugLog.i(TAG, "netRespondDomainBean->" + respondDomainBean.toString());

              // 一切OK
              handler.post(new Runnable() {

                @Override
                public void run() {
                  if (!netRequestIsCancelled.isCancelled()) {
                    if (domainBeanAsyncHttpResponseListener != null) {
                      domainBeanAsyncHttpResponseListener.onSuccess(respondDomainBean);
                    }
                  } else {
                    DebugLog.i(TAG, "<<<<<<<<<<     发起的 DomainBean [" + abstractFactoryMappingKey + "] 网络请求, 已经被取消    >>>>>>>>>>");
                  }

                  // 通知控制层, 本次网络请求彻底完成
                  if (domainBeanAsyncHttpResponseListener instanceof IDomainBeanAsyncHttpResponseListenerWithUIControl) {
                    ((IDomainBeanAsyncHttpResponseListenerWithUIControl) domainBeanAsyncHttpResponseListener).onEnd();
                  }
                }
              });

              // 一切OK
              return;
            } catch (Exception e) {
              DebugLog.e(TAG, "创建 网络响应业务Bean失败, 出现这种情况的业务Bean是 --> " + abstractFactoryMappingKey + ", 原因-->" + e.getLocalizedMessage());
              error.setErrorCodeEnum(MyNetErrorCodeEnum.kNetErrorCodeEnum_Server_ParseNetRespondStringToDomainBeanFailed);
              error.setErrorMessage("将网络返回的数据字符串解析成业务Bean失败.");
            }
            // ------------------------------------- >>>
          } while (false);

          // 出现了错误
          handler.post(new Runnable() {

            @Override
            public void run() {
              if (!netRequestIsCancelled.isCancelled()) {
                if (domainBeanAsyncHttpResponseListener != null) {
                  domainBeanAsyncHttpResponseListener.onFailure(error);
                }
              } else {
                DebugLog.i(TAG, "<<<<<<<<<<     发起的 DomainBean [" + abstractFactoryMappingKey + "] 网络请求, 已经被取消    >>>>>>>>>>");
              }

              // 通知控制层, 本次网络请求彻底完成
              if (domainBeanAsyncHttpResponseListener instanceof IDomainBeanAsyncHttpResponseListenerWithUIControl) {
                ((IDomainBeanAsyncHttpResponseListenerWithUIControl) domainBeanAsyncHttpResponseListener).onEnd();
              }
            }
          });

        }

        @Override
        public void onFailure(final INetRequestIsCancelled netRequestIsCancelled, final int statusCode, final Throwable e) {
          DebugLog.e(TAG, "<<<<<<<<<<     发起的 DomainBean [" + abstractFactoryMappingKey + "] 网络请求, 网络层访问失败 , 原因-->" + e.getLocalizedMessage());

          handler.post(new Runnable() {

            @Override
            public void run() {
              if (!netRequestIsCancelled.isCancelled()) {
                if (domainBeanAsyncHttpResponseListener != null) {
                  domainBeanAsyncHttpResponseListener.onFailure(new MyNetRequestErrorBean(statusCode, e.getLocalizedMessage()));
                }
              } else {
                DebugLog.i(TAG, "<<<<<<<<<<     发起的 DomainBean [" + abstractFactoryMappingKey + "] 网络请求, 已经被取消    >>>>>>>>>>");
              }

              // 通知控制层, 本次网络请求彻底完成
              if (domainBeanAsyncHttpResponseListener instanceof IDomainBeanAsyncHttpResponseListenerWithUIControl) {
                ((IDomainBeanAsyncHttpResponseListenerWithUIControl) domainBeanAsyncHttpResponseListener).onEnd();
              }
            }
          });
        }
      });

      DebugLog.i(TAG, "         ----- 参数检验正确, 启动子线程进行异步访问.  -----          ");

      // 通知控制层, 本次网络请求参数正确, 可以正常发起
      if (domainBeanAsyncHttpResponseListener instanceof IDomainBeanAsyncHttpResponseListenerWithUIControl) {
        ((IDomainBeanAsyncHttpResponseListenerWithUIControl) domainBeanAsyncHttpResponseListener).onBegin();
      }

    } catch (Exception e) {
      DebugLog.e(TAG, "发起网络请求失败, 错误原因-->" + e.getLocalizedMessage());
    } finally {

    }

    //
    DebugLog.i(TAG, " ");
    DebugLog.i(TAG, " ");
    DebugLog.i(TAG, " ");
    DebugLog.i(TAG, " ");

    return requestHandle;
  }

  /**
   * 请求一个文件下载
   * 
   * @param url
   *          文件下载url
   * @param fileFullSavePath
   *          文件完整的保存路径(包括文件名)
   * @param isNeedContinuingly
   *          是否需要断点续传
   * @param fileAsyncHttpResponseListener
   *          文件下载异步监听
   */
  public INetRequestHandle requestFileDownlaod(final String url, final Map<String, String> dataDictionary, final String fileFullSavePath, final boolean isNeedContinuingly,
      final IFileAsyncHttpResponseListener fileAsyncHttpResponseListener) {

    INetRequestHandle requestHandle = null;

    final File file = new File(fileFullSavePath);

    requestHandle = HttpEngineFactoryMethodSingleton.getInstance.requestFile(url, isNeedContinuingly, dataDictionary, file, new IFileRequestAsyncHttpResponseListener() {

      @Override
      public void onSuccess(final File file) {
        fileAsyncHttpResponseListener.onSuccess(file);
      }

      @Override
      public void onFailure(int statusCode, Throwable e) {
        fileAsyncHttpResponseListener.onFailure(new MyNetRequestErrorBean(statusCode, e.getLocalizedMessage()));
      }

      @Override
      public void onProgress(final long bytesWritten, final long totalSize) {
        DebugLog.i(TAG, "文件下载进度-->bytesWritten=" + bytesWritten + ", totalSize=" + totalSize);
        // 通知外部, 下载进度
        if (fileAsyncHttpResponseListener != null) {
          final long tempBytesWritten = bytesWritten;// 当前进度

          // 如果需要在后台线程中通知下载进度
          if (fileAsyncHttpResponseListener instanceof IFileAsyncHttpResponseListenerOnProgressDoInBackground) {
            ((IFileAsyncHttpResponseListenerOnProgressDoInBackground) fileAsyncHttpResponseListener).onProgressDoInBackground(tempBytesWritten, totalSize);
          }

          // 如果需要在主线程中通知下载进度
          if (fileAsyncHttpResponseListener instanceof IFileAsyncHttpResponseListenerOnProgressDoInUIThread) {
            handler.post(new Runnable() {

              @Override
              public void run() {
                ((IFileAsyncHttpResponseListenerOnProgressDoInUIThread) fileAsyncHttpResponseListener).onProgressDoInUIThread(tempBytesWritten, totalSize);
              }
            });
          }

        }
      }
    });
    return requestHandle;
  }

  /**
   * 请求下载文件
   * 
   * @param url
   * @param fileFullSavePath
   * @param fileAsyncHttpResponseHandler
   */
  public INetRequestHandle requestFileDownlaod(final String url, final String fileFullSavePath, final IFileAsyncHttpResponseListener fileAsyncHttpResponseHandler) {
    return requestFileDownlaod(url, null, fileFullSavePath, false, fileAsyncHttpResponseHandler);
  }

  /**
   * 下载一本书籍, 为今日书院项目特殊准备的
   * 
   * @param url
   * @param bindAccount
   * @param fileFullSavePath
   * @param fileAsyncHttpResponseListener
   */
  public INetRequestHandle requestBookDownlaod(final String url, final LoginNetRespondBean bindAccount, final String fileFullSavePath,
      final IFileAsyncHttpResponseListener fileAsyncHttpResponseListener) {
    INetRequestHandle requestHandle = null;
    if (TextUtils.isEmpty(url) || TextUtils.isEmpty(fileFullSavePath) || fileAsyncHttpResponseListener == null || bindAccount == null) {
      DebugLog.e(TAG, "入参为空.");
      return requestHandle;
    }

    // 构造 POST HttpEntity
    Map<String, String> body = new HashMap<String, String>();
    // 登录用户名和密码
    body.put("user_id", bindAccount.getUser());
    body.put("user_password", bindAccount.getPassword());
    return requestFileDownlaod(url, body, fileFullSavePath, true, fileAsyncHttpResponseListener);
  }

}
