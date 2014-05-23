package cn.retech.theme_manage;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserFactory;

import android.content.Context;
import android.os.AsyncTask;
import cn.retech.domainbean_model.theme.ThemeNetRequestBean;
import cn.retech.domainbean_model.theme.ThemeNetRespondBean;
import cn.retech.global_data_cache.GlobalDataCacheForMemorySingleton;
import cn.retech.global_data_cache.GlobalDataCacheForNeedSaveToFileSystem;
import cn.retech.global_data_cache.LocalCacheDataPathConstant;
import cn.retech.my_network_engine.IDomainBeanAsyncHttpResponseListener;
import cn.retech.my_network_engine.IFileAsyncHttpResponseListenerOnProgressDoInUIThread;
import cn.retech.my_network_engine.INetRequestHandle;
import cn.retech.my_network_engine.SimpleNetworkEngineSingleton;
import cn.retech.my_network_engine.net_error_handle.MyNetRequestErrorBean;
import cn.retech.theme_manage.ThemeModel.ThemeBuilder;
import cn.retech.toolutils.DebugLog;
import cn.retech.toolutils.SimpleZipTools;
import cn.retech.toolutils.ToolsFunctionForThisProgect;

public enum ThemeManagerSingleton {
  getInstance;
  private final String TAG = this.getClass().getSimpleName();
  private ThemeModel newThemeModel = null;
  private final String THEME_NAME = "theme.zip";
  private Context context;

  public void setContext(Context context) {
    this.context = context;
  }

  public ThemeModel getNewThemeModel() {
    return newThemeModel;
  }

  /**
   * 获取主题包完整路径
   * 
   * @param themeNetRespondBean
   * @return
   */
  private String getThemeFullPath(ThemeNetRespondBean themeNetRespondBean) {
    return LocalCacheDataPathConstant.themeCachePathInDevice() + "/" + themeNetRespondBean.getCompanyid() + "/" + THEME_NAME;
  }

  /**
   * 获取主题包父路径
   * 
   * @param themeNetRespondBean
   * @return
   */
  private String getThemeParentPath(String companyid) {
    return LocalCacheDataPathConstant.themeCachePathInDevice() + "/" + companyid;
  }

  private static IThemeDownloadResponseHandler iDownloadResponseHandler;

  public void requestThemeWithUserName(final String userID, final String userPassWord, final IThemeDownloadResponseHandler iThemeDownloadResponseHandler) {
    ThemeManagerSingleton.iDownloadResponseHandler = iThemeDownloadResponseHandler;
    // 联网获取皮肤包的url
    ThemeNetRequestBean themeNetRequestBean = new ThemeNetRequestBean(userID, userPassWord);
    @SuppressWarnings("unused")
    INetRequestHandle netRequestRequestForTheme = SimpleNetworkEngineSingleton.getInstance.requestDomainBean(themeNetRequestBean, new IDomainBeanAsyncHttpResponseListener() {

      @Override
      public void onSuccess(Object respondDomainBean) {
        final ThemeNetRespondBean themeNetRespondBean = (ThemeNetRespondBean) respondDomainBean;
        // 判断是否需要下载主题包
        if (!themeNetRespondBean.isValidate()) {
          DebugLog.i(TAG, ">>>>>>>>>>>>>>>>>>>解压Assets路径下的主题包<<<<<<<<<<<<<<<<<<<");
          // 解压文件+替换文件
          File defaultFile = new File(LocalCacheDataPathConstant.themeCachePathInDevice() + "/default");
          if (!defaultFile.exists()) {
            defaultFile.mkdir();
          }
          GlobalDataCacheForMemorySingleton.getInstance.setCompanyid("default");
          iThemeDownloadResponseHandler.onDownloadSuccess();
          // 文件路径,输出路径
          UnzipThemeZipPackageToCacheAsyncTask unzipThemeZipPackageToCacheAsyncTask = new UnzipThemeZipPackageToCacheAsyncTask();
          unzipThemeZipPackageToCacheAsyncTask.execute(defaultFile + "/theme.zip", defaultFile.getAbsolutePath(), "default");
        } else if (isHadNewTheme(themeNetRespondBean)) {
          // 将companyid存储到缓存中
          GlobalDataCacheForMemorySingleton.getInstance.setCompanyid(themeNetRespondBean.getCompanyid());
          // 下载主题包
          DebugLog.i(TAG, ">>>>>>>>>>>>>>>>>>>下载主题包<<<<<<<<<<<<<<<<<<<");
          SimpleNetworkEngineSingleton.getInstance.requestFileDownlaod(themeNetRespondBean.getUrl(), getThemeFullPath(themeNetRespondBean), new IFileAsyncHttpResponseListenerOnProgressDoInUIThread() {

            @Override
            public void onSuccess(File file) {
              // 文件下载完成需要通知外层
              ThemeManagerSingleton.iDownloadResponseHandler.onDownloadSuccess();
              // 解压文件+替换文件
              UnzipThemeZipPackageToCacheAsyncTask unzipThemeZipPackageToCacheAsyncTask = new UnzipThemeZipPackageToCacheAsyncTask();
              unzipThemeZipPackageToCacheAsyncTask.execute(getThemeFullPath(themeNetRespondBean), getThemeParentPath(themeNetRespondBean.getCompanyid()), themeNetRespondBean.getCompanyid());
            }

            @Override
            public void onFailure(MyNetRequestErrorBean error) {
              DebugLog.e(TAG, error.getErrorMessage());
              iThemeDownloadResponseHandler.onFailure(error);
            }

            @Override
            public void onProgressDoInUIThread(final long bytesWritten, final long totalSize) {
              iThemeDownloadResponseHandler.onProgressDoInBackground(bytesWritten, totalSize);
            }
          });
        } else {
          loadLocalThemeByCompanyID(themeNetRespondBean.getCompanyid());
        }
        GlobalDataCacheForNeedSaveToFileSystem.writeCompanyidToFileSystem();
      }

      @Override
      public void onFailure(MyNetRequestErrorBean error) {

        // 获取主题信息出错则需要提示用户.
        iThemeDownloadResponseHandler.onFailure(error);

      }
    });
  }

  /**
   * 判断是否需要下载主题
   * 
   * @param themeNetRespondBean
   * @return
   */
  private boolean isHadNewTheme(ThemeNetRespondBean themeNetRespondBean) {

    // 从本地缓存获取文件的路径
    File file = new File(LocalCacheDataPathConstant.themeCachePathInDevice() + "/" + themeNetRespondBean.getCompanyid());
    do {
      // 首先判断路径是否存在
      if (!file.exists()) {
        file.mkdir();
        break;
      }
      // 读取内存中主题配置文件 theme_config.xml
      File xmlFile = new File(getThemeParentPath(themeNetRespondBean.getCompanyid()) + "/theme_config.xml");

      if (!xmlFile.exists()) {
        break;
      }
      try {
        // 将文件转为string然后转为byte并创建输入流
        InputStream inputStreamOfRespondData = new FileInputStream(xmlFile);

        XmlPullParserFactory parserCreator = XmlPullParserFactory.newInstance();
        XmlPullParser parser = parserCreator.newPullParser();
        parser.setInput(inputStreamOfRespondData, "utf-8");// 设置数据源编码
        DebugLog.e(TAG, parser.toString());
        int parserEvent = parser.getEventType();// 获取事件类型
        while (parserEvent != XmlPullParser.END_DOCUMENT) {

          switch (parserEvent) {
            case XmlPullParser.START_TAG:// 开始元素事件
              // // 平台(最外层)
              if (parser.getName().equalsIgnoreCase("platform")) {

                do {
                  // 平台名称(是platform的属性)获取值判断是否是Android平台的
                  if (!parser.getAttributeValue(null, ThemeConstant.ThemeNode.name.name()).equals(ThemeConstant.ThemeNode.Android.name())) {
                    break;
                  }
                  // 判断公司ID
                  if (!parser.getAttributeValue(null, ThemeConstant.ThemeNode.companyid.name()).equals(themeNetRespondBean.getCompanyid())) {
                    break;
                  }
                  // 判断主题版本
                  if (!parser.getAttributeValue(null, ThemeConstant.ThemeNode.version.name()).equals(themeNetRespondBean.getVersion())) {
                    break;
                  }
                } while (false);
              }
              break;

            case XmlPullParser.END_TAG:// 结束元素事件
              break;
          }

          parserEvent = parser.next();
        }
        inputStreamOfRespondData.close();
      } catch (Exception e) {
        e.printStackTrace();
      }
      GlobalDataCacheForMemorySingleton.getInstance.setCompanyid(themeNetRespondBean.getCompanyid());
      return false;
    } while (false);

    return true;
  }

  public void loadLocalThemeByCompanyID(String companyid) {
    DebugLog.i(TAG, ">>>>>>>>>>>>>>>>>>>替换主题<<<<<<<<<<<<<<<<<<<");
    ThemeModel.ThemeBuilder themeModel = new ThemeBuilder();
    ThemeNetRespondBean themeNetRespondBean = new ThemeNetRespondBean(GlobalDataCacheForMemorySingleton.getInstance.getUsernameForLastSuccessfulLogon(), "", "",
        GlobalDataCacheForMemorySingleton.getInstance.getCompanyid(), false);
    newThemeModel = themeModel.buildTheme(themeNetRespondBean, getThemeParentPath(companyid));
    if (newThemeModel != null) {
      if (ThemeManagerSingleton.iDownloadResponseHandler != null) {

        // 当文件替换完成,并替换成功需要通知上层
        ThemeManagerSingleton.iDownloadResponseHandler.onMissionComplete();
      }
      DebugLog.e(TAG, newThemeModel.getThemeBean().toString());
    } else {
      // TODO : 如果加载指定公司的主题包失败的话, 就加载本地默认的主题包
    }
  }

  private class UnzipThemeZipPackageToCacheAsyncTask extends AsyncTask<String, Integer, String> {
    private int entryCount;

    @Override
    protected void onPreExecute() {

    }

    @Override
    protected String doInBackground(String... params) {

      String resultString = "Success";

      do {
        try {
          if (params[2].equals("default")) {
            ToolsFunctionForThisProgect.copyBigDataToSD(context, "theme.zip", params[1] + "/theme.zip");
            if (!SimpleZipTools.unZipFiles(params[0], params[1])) {
              resultString = "Fail";
              break;
            }

          } else {
            if (!SimpleZipTools.unZipFiles(params[0], params[1])) {
              resultString = "Fail";
              break;
            }
          }

        } catch (Exception e) {
          e.printStackTrace();
        }
      } while (false);
      return resultString;
    }

    @Override
    protected void onProgressUpdate(Integer... values) {
      int progress = values[0] * 100 / entryCount;
      DebugLog.e(TAG, progress + "");
    }

    @Override
    protected void onPostExecute(String result) {
      if (result.equals("Success")) {
        DebugLog.i(TAG, ">>>>>>>>>>>>>>>>>>>文件解压完成<<<<<<<<<<<<<<<<<<<");
        // 2,替换主题
        loadLocalThemeByCompanyID(GlobalDataCacheForMemorySingleton.getInstance.getCompanyid());
      } else {
        DebugLog.e(TAG, "解压书籍失败!");
      }
    }
  }

}
