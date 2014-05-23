package cn.retech.domainbean_model.book;

import java.io.File;
import java.util.Observable;

import android.os.AsyncTask;
import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;
import cn.retech.domainbean_model.booklist_in_bookstores.BookInfo;
import cn.retech.domainbean_model.get_book_download_url.GetBookDownloadUrlNetRequestBean;
import cn.retech.domainbean_model.get_book_download_url.GetBookDownloadUrlNetRespondBean;
import cn.retech.domainbean_model.login.LoginNetRespondBean;
import cn.retech.global_data_cache.GlobalDataCacheForMemorySingleton;
import cn.retech.global_data_cache.GlobalDataCacheForNeedSaveToFileSystem;
import cn.retech.global_data_cache.LocalCacheDataPathConstant;
import cn.retech.my_network_engine.IDomainBeanAsyncHttpResponseListener;
import cn.retech.my_network_engine.IFileAsyncHttpResponseListenerOnProgressDoInUIThread;
import cn.retech.my_network_engine.INetRequestHandle;
import cn.retech.my_network_engine.SimpleNetworkEngineSingleton;
import cn.retech.my_network_engine.net_error_handle.MyNetErrorCodeEnum;
import cn.retech.my_network_engine.net_error_handle.MyNetRequestErrorBean;
import cn.retech.toolutils.DebugLog;
import cn.retech.toolutils.SimpleSDCardTools;
import cn.retech.toolutils.SimpleZipTools;

public final class Book extends Observable implements Cloneable {

  private final String TAG = this.getClass().getSimpleName();

  private Handler handler = new Handler(Looper.getMainLooper());
  private INetRequestHandle netRequestHandleForGetBookDownloadUrl;
  private INetRequestHandle netRequestHandleForDownloadBook;

  public static enum ObserverEnum {
    // 书籍解压进度
    kBookDecompressProgress,
    // 书籍下载进度
    kBookDownloadProgress,
    // 书籍状态
    kBookState
  }

  // 书籍状态枚举
  public static enum BookStateEnum {

    // 收费
    kBookStateEnum_Unpaid,
    // 支付中....
    kBookStateEnum_Paiding,
    // 已支付
    kBookStateEnum_Paid,
    // 免费
    kBookStateEnum_Free,
    // 有可以更新的内容
    kBookStateEnum_Update,

    // 等待下载中...(此时并没有调用网络引擎发起网络请求)
    kBookStateEnum_WaitForDownload,

    // 正在获取用于书籍下载的URL中...
    kBookStateEnum_GetBookDownloadUrl,
    // 正在下载中...
    kBookStateEnum_Downloading,
    // 暂停(也就是未下载完成, 可以进行断电续传)
    kBookStateEnum_Pause,
    // 未安装(已经下载完成, 还未完成安装)
    kBookStateEnum_NotInstalled,
    // 解压书籍zip资源包中....
    kBookStateEnum_Unziping,
    // 已安装(已经解压开的书籍, 可以正常阅读了)
    kBookStateEnum_Installed

  };

  public Book(BookInfo info) {
    // 进行 "数据保护"
    this.info = info.clone();
    double price = 0.0;
    try {
      if (TextUtils.isEmpty(info.getPrice())) {
        return;
      }
      price = Double.valueOf(info.getPrice());
    } catch (NumberFormatException e) {
      price = 0.0;
    }
    if (price > 0) {
      state = BookStateEnum.kBookStateEnum_Unpaid;
    } else {
      state = BookStateEnum.kBookStateEnum_Free;
    }

  }

  @Override
  public Book clone() {
    Book clone = null;
    try {
      clone = (Book) super.clone();

      // 书籍信息
      clone.info = this.info.clone();
      // 书籍当前状态
      clone.state = this.state;
      // 和当前书籍绑定的账号
      clone.bindAccount = this.bindAccount.clone();
      // 是否来自SD卡共享目录
      clone.isFromSharedDirectory = this.isFromSharedDirectory;
      // 书籍压缩包文件存目录
      clone.bookTmpZipResFilePath = this.bookTmpZipResFilePath;
    } catch (CloneNotSupportedException e) {
      throw new RuntimeException(e); // won't happen        
    }
    return clone;
  }

  // 书籍是否来自 "SD卡 共享目录"
  private boolean isFromSharedDirectory = false;

  public boolean isFromSharedDirectory() {
    return isFromSharedDirectory;
  }

  public void setFromSharedDirectory(boolean isFromSharedDirectory) {
    this.isFromSharedDirectory = isFromSharedDirectory;
  }

  // 书籍信息(从服务器获取的, 这个属性在初始化 LocalBook 时被赋值, 之后就是只读数据了)
  private BookInfo info;

  public BookInfo getInfo() {
    return info;
  }

  public void setInfo(BookInfo info) {
    this.info = info;
  }

  // 解压进度, 100% 数值是 1, 外部可以这样计算完成百分比 downloadProgress * 100
  private int decompressProgress;

  private void setDecompressProgress(int decompressProgress) {
    this.decompressProgress = decompressProgress;
    handler.post(new Runnable() {
      @Override
      public void run() {
        setChanged();
        notifyObservers(ObserverEnum.kBookDecompressProgress);
      }
    });

  }

  public int getDecompressProgress() {
    return decompressProgress;
  }

  // 下载进度, 100% 数值是 1, 外部可以这样计算完成百分比 downloadProgress * 100
  private float downloadProgress;

  private void setDownloadProgress(float downloadProgress) {
    this.downloadProgress = downloadProgress;
    handler.post(new Runnable() {

      @Override
      public void run() {
        setChanged();
        notifyObservers(ObserverEnum.kBookDownloadProgress);
        clearChanged();
      }
    });

  }

  public float getDownloadProgress() {
    return downloadProgress;
  }

  // 书籍状态
  private BookStateEnum state;

  public BookStateEnum getState() {
    return state;
  }

  public void setState(BookStateEnum state) {
    this.state = state;
    handler.post(new Runnable() {

      @Override
      public void run() {
        setChanged();
        notifyObservers(ObserverEnum.kBookState);
      }
    });

  }

  // 书籍保存文件夹路径
  public String bookSaveDirPath() {
    return LocalCacheDataPathConstant.localBookCachePathInSDCard() + "/" + info.getContent_id();
  }

  private static final String kTmpDownloadBookFileName = "tmp.zip";

  // 书籍压缩包文件存目录
  private String bookTmpZipResFilePath;

  public void setBookTmpZipResFilePath(String bookTmpZipResFilePath) {
    this.bookTmpZipResFilePath = bookTmpZipResFilePath;
  }

  public String getBookTmpZipResFilePath() {
    if (TextUtils.isEmpty(bookTmpZipResFilePath)) {
      bookTmpZipResFilePath = bookSaveDirPath() + "/" + kTmpDownloadBookFileName;
    }
    return bookTmpZipResFilePath;
  }

  // 删除书籍下载临时文件
  private void removeBookTmpZipResFile() {
    File file = new File(getBookTmpZipResFilePath());
    if (file.exists()) {
      if (!file.delete()) {
        DebugLog.e(TAG, "删除缓存的未下载完成的书籍数据失败!");
      }
    }
  }

  // 从书城中, 点击一本还未下载的书籍时, 这本书籍会被加入
  // "本地书籍列表(在 GlobalDataCacheForMemorySingleton->localBookList 中保存)"
  // 目前有两个需求:
  // 1) 当A账户登录书城下载书籍时, 如果此时A账户退出了(或者被B账户替换了), 那么就要暂停正在进行下载的所有跟A账户绑定的书籍;
  // 这里考虑的一点是, 如果A/B账户切换时, 当前账户是希望独享下载网速的.
  // 但是, 对于跟 "公共账户" 绑定的书籍, 是不需要停止下载的.
  // 2) 已经存在于 "本地书籍列表" 中的未下载完成的书籍, 再次进行断点续传时, 需要将跟这本书绑定的账号信息传递给服务器,
  // 才能获取到最新的书籍下载地址.
  // 因为服务器为了防止盗链, 所以每次断点续传时, 都需要重新获取目标书籍的最新下载地址.
  private LoginNetRespondBean bindAccount;

  public LoginNetRespondBean getBindAccount() {
    return bindAccount;
  }

  public void setBindAccount(LoginNetRespondBean bindAccount) {
    this.bindAccount = bindAccount;
  }

  // 设置当前书籍最新的版本(可以通过书籍的版本来确定服务器是否有可以下载的更新包)
  public void setBookVersion(String bookLatestVersion) {

  };

  // 付费之后的收据
  private byte[] receipt;

  // 开始下载一本书籍
  public boolean startDownloadBook() {
    return startDownloadBookWithReceipt(receipt);
  }

  // 开始下载一本书籍(需要 "收据", 对应那种收费的书籍)
  public boolean startDownloadBookWithReceipt(byte[] receipt) {
    do {
      if (state != BookStateEnum.kBookStateEnum_Paid && state != BookStateEnum.kBookStateEnum_Free && state != BookStateEnum.kBookStateEnum_Update && state != BookStateEnum.kBookStateEnum_Pause
          && state != BookStateEnum.kBookStateEnum_WaitForDownload) {
        // 只有书籍处于 Paid / Free / Update / Pause / WaitForDownload 状态时,
        // 才有可能触发书籍下载
        break;
      }

      // TODO : 后续在进行修改
      if (netRequestHandleForGetBookDownloadUrl != null && !netRequestHandleForGetBookDownloadUrl.idle()) {
        // 已经在获取书籍的URL, 不需要重复发起书籍下载请求
        break;
      }

      if (GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList().downlaodingBookNumber() >= 3) {
        // 更新书籍状态->WaitForDownload
        // 要控制同时并发下载的书籍的数量, 防止阻塞正常的网络访问.
        if (state != BookStateEnum.kBookStateEnum_WaitForDownload) {
          setState(BookStateEnum.kBookStateEnum_WaitForDownload);
        }

        this.receipt = receipt;
        break;
      }

      GetBookDownloadUrlNetRequestBean netRequestBeanForGetBookDownloadUrl = new GetBookDownloadUrlNetRequestBean(getInfo().getContent_id(), getBindAccount());
      netRequestBeanForGetBookDownloadUrl.setReceipt(receipt);
      netRequestHandleForGetBookDownloadUrl = SimpleNetworkEngineSingleton.getInstance.requestDomainBean(netRequestBeanForGetBookDownloadUrl, new IDomainBeanAsyncHttpResponseListener() {

        @Override
        public void onFailure(MyNetRequestErrorBean error) {
          DebugLog.e(TAG, "获取书籍下载URL失败." + error.toString());

          // 激活另一本处于下载等待状态的书籍
          GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList().startDownloadForWaitStateBookWithIgnoreBook(Book.this);

          //
          bookDownloadOrUnzipErrorHandlerWithMessage(error.getErrorMessage());

        }

        @Override
        public void onSuccess(Object respondDomainBean) {
          DebugLog.i(TAG, "获取要下载的书籍URL 成功!");
          GetBookDownloadUrlNetRespondBean netRespondBeanForGetBookDownloadUrl = (GetBookDownloadUrlNetRespondBean) respondDomainBean;
          downloadBookWithURLString(netRespondBeanForGetBookDownloadUrl.getBookDownloadUrl());
        }
      });

      // 更新书籍状态->GetBookDownloadUrl
      setState(BookStateEnum.kBookStateEnum_GetBookDownloadUrl);
      return true;
    } while (false);

    return false;
  }

  // 开始下载一本书籍(为了防止盗链, 所以每次下载书籍时的URL都是一次性的)
  private boolean downloadBookWithURLString(final String urlString) {
    String messageOfCustomError = "";
    do {
      if (!SimpleSDCardTools.isHasSDCard()) {
        messageOfCustomError = "SD卡不存在!";
        break;
      }

      if (TextUtils.isEmpty(urlString)) {
        assert false : "入参urlString为空!";
        messageOfCustomError = "入参urlString为空!";
        break;
      }
      DebugLog.i(TAG, "要下载的书籍URL = " + urlString);

      DebugLog.i(TAG, "开始下载书籍 : " + info.getName() + ", id = " + info.getContent_id());
      if (netRequestHandleForDownloadBook != null) {
        netRequestHandleForDownloadBook.cancel();
      }
      // 开始下载目标书籍 bookTmpZipResFilePath()
      netRequestHandleForDownloadBook = SimpleNetworkEngineSingleton.getInstance.requestBookDownlaod(urlString, bindAccount, getBookTmpZipResFilePath(),
          new IFileAsyncHttpResponseListenerOnProgressDoInUIThread() {

            @Override
            public void onFailure(final MyNetRequestErrorBean error) {
              DebugLog.e(TAG, "书籍下载失败 error=" + error.toString());

              // 激活另一本处于下载等待状态的书籍
              GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList().startDownloadForWaitStateBookWithIgnoreBook(Book.this);

              bookDownloadOrUnzipErrorHandlerWithMessage(error.getErrorMessage());

              if (error.getErrorCodeEnum() != MyNetErrorCodeEnum.kNetErrorCodeEnum_Client_TimeOut) {
                // 删除临时文件
                removeBookTmpZipResFile();
              }

            }

            @Override
            public void onProgressDoInUIThread(final long bytesWritten, final long totalSize) {

              float pro = (float) bytesWritten / totalSize * 100.0f;

              setDownloadProgress(pro);
            }

            @Override
            public void onSuccess(final File file) {
              DebugLog.i(TAG, "书籍下载成功 : " + info.getName() + ", id = " + info.getContent_id());

              receipt = null;

              // 激活另一本处于下载等待状态的书籍
              GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList().startDownloadForWaitStateBookWithIgnoreBook(Book.this);

              // 更新书籍状态 --> Unziping
              setState(BookStateEnum.kBookStateEnum_Unziping);

              // 当书籍下载成功后自动在后台进行解压操作
              UnzipBookZipPackageToSDCardAsyncTask unzipBookZipPackageToSDCardAsyncTask = new UnzipBookZipPackageToSDCardAsyncTask();
              unzipBookZipPackageToSDCardAsyncTask.execute();
            }
          });

      // 更新书籍状态->Downloading
      setState(BookStateEnum.kBookStateEnum_Downloading);
      return true;
    } while (false);

    // 启动下载一本书籍的操作失败
    DebugLog.e(TAG, "启动一本书籍下载失败，原因:" + messageOfCustomError);
    bookDownloadOrUnzipErrorHandlerWithMessage(messageOfCustomError);
    return false;
  }

  // 停止下载一本书籍
  public void stopDownloadBook() {
    if (state != BookStateEnum.kBookStateEnum_Downloading && state != BookStateEnum.kBookStateEnum_GetBookDownloadUrl && state != BookStateEnum.kBookStateEnum_WaitForDownload) {
      // 只有处于 "Downloading / GetBookDownloadUrl / WaitForDownload" 状态的书籍,
      // 才能被暂停.
      return;
    }

    // 先取消之前的网络请求
    if (netRequestHandleForGetBookDownloadUrl != null) {
      netRequestHandleForGetBookDownloadUrl.cancel();
    }
    if (netRequestHandleForDownloadBook != null) {
      netRequestHandleForDownloadBook.cancel();
    }

    // 更新书籍状态->Pause
    setState(BookStateEnum.kBookStateEnum_Pause);
    // 激活另一本处于下载等待状态的书籍
    GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList().startDownloadForWaitStateBookWithIgnoreBook(this);
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + ((info.getContent_id() == null) ? 0 : info.getContent_id().hashCode());
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj)
      return true;
    if (obj == null)
      return false;
    if (getClass() != obj.getClass())
      return false;
    Book other = (Book) obj;
    if (info == null) {
      if (other.info != null)
        return false;
    } else if (!info.getContent_id().equals(other.getInfo().getContent_id()))
      return false;
    return true;
  }

  // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  private class UnzipBookZipPackageToSDCardAsyncTask extends AsyncTask<String, Integer, String> {
    private int entryCount;

    @Override
    protected void onPreExecute() {

    }

    @Override
    protected String doInBackground(String... params) {
      String resultString = "Success";

      do {
        if (!unzipBookZipResToSDCard()) {
          resultString = "Fail";
          break;
        }

        removeBookTmpZipResFile();
      } while (false);
      // 删除临时文件

      return resultString;
    }

    @Override
    protected void onProgressUpdate(Integer... values) {
      int progress = values[0] * 100 / entryCount;
      setDecompressProgress(progress);
    }

    @Override
    protected void onPostExecute(String result) {
      if (result.equals("Success")) {
        setState(BookStateEnum.kBookStateEnum_Installed);
        GlobalDataCacheForNeedSaveToFileSystem.saveLocalBookToFileSystem(Book.this);
        DebugLog.i(TAG, "书籍解压完成.");

      } else {
        // 更新书籍状态->未解压
        setState(BookStateEnum.kBookStateEnum_NotInstalled);
        DebugLog.e(TAG, "解压书籍失败!");
        bookDownloadOrUnzipErrorHandlerWithMessage("解压书籍失败.");
      }
    }

    private boolean unzipBookZipResToSDCard() {
      boolean isOK = true;
      try {
        // 删除书籍目录中全部文件, 除了kTmpDownloadBookFileName, 这是为了在解压之前先清理之前未解压成功的垃圾
        // ----- 暂时屏蔽, 因为如果在删除过程中退出app了, 会什么结果, 不好判断
        // ImmutableSet<String> needKeepFileNameSet = ImmutableSet.of(kTmpDownloadBookFileName, "LocalBook");
        // SimpleFileTools.delAllFileExceptForSpecialFiles(bookSaveDirPath(), needKeepFileNameSet);
        // -----
        // 解压书籍
        SimpleZipTools.unzip(getBookTmpZipResFilePath(), bookSaveDirPath());
      } catch (Exception e) {
        DebugLog.e(TAG, e.toString());
        isOK = false;
      }

      return isOK;
    }
  }

  // 书籍下载解压过程中, 如果发生错误时, 通知控制层的块
  public interface IBookDownloadErrorBlockHandler {
    public void onError(final String errorMessage);
  }

  private IBookDownloadErrorBlockHandler bookDownloadErrorBlockHandler;

  // 书籍下载/解压过程中发生了错误时的处理方法
  // 此方法作用 :
  // 1. 通知外层发生了错误
  // 2. 复位书籍状态-->Pause, 好让用户可以重新下载
  public void setBookDownloadErrorBlockHandler(IBookDownloadErrorBlockHandler bookDownloadErrorBlockHandler) {
    this.bookDownloadErrorBlockHandler = bookDownloadErrorBlockHandler;
  }

  private void bookDownloadOrUnzipErrorHandlerWithMessage(final String message) {
    if (bookDownloadErrorBlockHandler != null) {

      // 通知外层, 发生了错误
      bookDownloadErrorBlockHandler.onError(message);
    }

    // 复位下载进度, 此时不需要通知外层
    downloadProgress = 0;

    //
    if (isFromSharedDirectory) {
      // 如果书籍来自 SD卡共享目录", 就复位书籍状态为 "NotInstalled"
      setState(BookStateEnum.kBookStateEnum_NotInstalled);
    } else {
      // 否则书籍就是来自于服务器, 就复位当前书籍状态, 好让用户可以重新下载
      setState(BookStateEnum.kBookStateEnum_Pause);
    }
  }

  // 解压一本书籍(只有当上次解压一本书籍, 没有完成时, 退出了app, 此时app的状态为 kBookStateEnum_Unziping 时,
  // 这个方法才有意义
  public void unzipBook() {
    String messageOfCustomError = "";
    do {
      if (state != BookStateEnum.kBookStateEnum_NotInstalled) {
        // 如果当前书籍不是 NotInstalled(未安装), 那么这个方法无效
        messageOfCustomError = "当前书籍状态不是 未安装 状态.";
        break;
      }

      File bookTmpZipResFile = new File(getBookTmpZipResFilePath());
      if (!bookTmpZipResFile.exists()) {
        // 如果书籍临时压缩包已经不存在了, 此方法也无效
        messageOfCustomError = "书籍临时压缩包文件丢失!";
        break;
      }

      // 更新书籍状态 --> Unziping
      setState(BookStateEnum.kBookStateEnum_Unziping);
      // 在后台线程中解压缩书籍zip资源包.
      UnzipBookZipPackageToSDCardAsyncTask unzipBookZipPackageToSDCardAsyncTask = new UnzipBookZipPackageToSDCardAsyncTask();
      unzipBookZipPackageToSDCardAsyncTask.execute();
      return;
    } while (false);

    // 复位书籍状态, 给用户重新下载书籍的机会
    bookDownloadOrUnzipErrorHandlerWithMessage(messageOfCustomError);
    return;
  }

}
