package cn.retech.global_data_cache;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import cn.retech.domainbean_model.book.Book;
import cn.retech.domainbean_model.book.Book.BookStateEnum;
import cn.retech.domainbean_model.book.BookFile;
import cn.retech.domainbean_model.book.BookList;
import cn.retech.domainbean_model.book.for_serializable.BookForSerializable;
import cn.retech.domainbean_model.booklist_in_bookstores.BookInfo;
import cn.retech.domainbean_model.login.LoginNetRespondBean;
import cn.retech.toolutils.DebugLog;
import cn.retech.toolutils.FunctionDeployment;
import cn.retech.toolutils.MD5Util;

/**
 * 这里序列化对象的保存目录是 : /data/data/<包名>/files/ , 这个目录会在用户在 "应用程序管理" 中点击 "清理数据" 按钮后被清理
 * 
 * @author computer
 * 
 */
public final class GlobalDataCacheForNeedSaveToFileSystem {
  private final static String TAG = GlobalDataCacheForNeedSaveToFileSystem.class.getSimpleName();

  private GlobalDataCacheForNeedSaveToFileSystem() {

  }

  private enum CacheDataNameForSaveToFile {
    // 书籍文件夹根目录
    RootBookFileDirectory,
    // 用户是否是首次启动App
    FirstStartApp,
    // 用户是否已经登录了
    IsLogged,
    // 用户最后一次成功登录时的用户名/密码
    UsernameForLastSuccessfulLogon,
    //
    PasswordForLastSuccessfulLogon,
    //
    LatestLogonNetRespondBean,
    //
    LocalBook,
    // 当前app版本号, 用了防止升级app时, 本地缓存的序列化数据恢复出错.
    LocalAppVersion,
    // 公司id
    companyid
  };

  public static synchronized void readAppConfingInfo() {
    //
    DebugLog.i(TAG, "start loading --> isFirstStartApp");
    SharedPreferences sharedPreferences = GlobalDataCacheForMemorySingleton.getInstance.getApplication().getSharedPreferences(GlobalConstant.APP_GLOBAL_SHARED_PREFERENCES, Context.MODE_PRIVATE);
    GlobalDataCacheForMemorySingleton.getInstance.setFirstStartApp(sharedPreferences.getBoolean(CacheDataNameForSaveToFile.FirstStartApp.name(), true));
  }

  /**
   * 读取本地缓存的 用户登录信息
   */
  public static void readUserLoginInfoToGlobalDataCacheForMemorySingleton() {
    // 用户登录相关信息
    DebugLog.i(TAG, "start loading --> 用户登录信息");
    SharedPreferences sharedPreferences = GlobalDataCacheForMemorySingleton.getInstance.getApplication().getSharedPreferences(GlobalConstant.APP_GLOBAL_SHARED_PREFERENCES, Context.MODE_PRIVATE);
    boolean isLogged = sharedPreferences.getBoolean(CacheDataNameForSaveToFile.IsLogged.name(), false);
    if (isLogged) {
      String usernameForLastSuccessfulLogon = sharedPreferences.getString(CacheDataNameForSaveToFile.UsernameForLastSuccessfulLogon.name(), null);
      String passwordForLastSuccessfulLogon = sharedPreferences.getString(CacheDataNameForSaveToFile.PasswordForLastSuccessfulLogon.name(), null);
      LoginNetRespondBean latestLogonNetRespondBean = (LoginNetRespondBean) deserializeObjectFromDeviceFileSystem(CacheDataNameForSaveToFile.LatestLogonNetRespondBean.name());
      GlobalDataCacheForMemorySingleton.getInstance.noteSignInSuccessfulInfo(latestLogonNetRespondBean, usernameForLastSuccessfulLogon, passwordForLastSuccessfulLogon);
    }
  }

  /**
   * 读取公司ID
   */
  public static void readCompanyidInfoToGlobalDataCacheForMemorySingleton() {
    // 用户登录相关信息
    DebugLog.i(TAG, "start loading --> 公司ID");
    SharedPreferences sharedPreferences = GlobalDataCacheForMemorySingleton.getInstance.getApplication().getSharedPreferences(GlobalConstant.APP_GLOBAL_SHARED_PREFERENCES, Context.MODE_PRIVATE);
    boolean isLogged = sharedPreferences.getBoolean(CacheDataNameForSaveToFile.IsLogged.name(), false);
    if (isLogged) {
      GlobalDataCacheForMemorySingleton.getInstance.setCompanyid(sharedPreferences.getString(CacheDataNameForSaveToFile.companyid.name(), null));
    }
  }

  /**
   * 读取本地缓存的 书籍列表
   */
  public static void readLocalBookListToGlobalDataCacheForMemorySingleton() {
    BookList localBookList = new BookList();
    // 检索 本地书籍安装目录下面的全部书籍
    File localBookCacheDirInSDCard = LocalCacheDataPathConstant.localBookCachePathInSDCard();
    for (File localBookCacheDir : localBookCacheDirInSDCard.listFiles()) {
      if (localBookCacheDir.isDirectory() && (localBookCacheDir.listFiles() == null || localBookCacheDir.listFiles().length <= 0)) {
        // 如果书籍保存文件夹是空的, 证明是无效的文件夹, 就及时删除
        localBookCacheDir.delete();
        continue;
      }
      for (File tempFile : localBookCacheDir.listFiles()) {
        if (tempFile.isFile() && tempFile.getName().equals(CacheDataNameForSaveToFile.LocalBook.name())) {
          FileInputStream fileInputStream = null;
          try {
            fileInputStream = new FileInputStream(tempFile);
          } catch (FileNotFoundException e) {
            e.printStackTrace();
            fileInputStream = null;
          }
          if (fileInputStream == null) {
            continue;
          }
          final BookForSerializable bookForSerializable = (BookForSerializable) deserializeObjectFromFile(fileInputStream);
          if (!BookForSerializable.isValid(bookForSerializable)) {
            // 证明本地保存的当前书籍是无效的
            continue;
          }
          Book book = new Book(bookForSerializable.getInfo());
          book.setState(bookForSerializable.getState());
          book.setBindAccount(bookForSerializable.getBindAccount());
          book.setFromSharedDirectory(bookForSerializable.isFromSharedDirectory());
          book.setBookTmpZipResFilePath(bookForSerializable.getBookTmpZipResFilePath());
          localBookList.addBook(book);
        }
      }
    }
    GlobalDataCacheForMemorySingleton.getInstance.setLocalBookList(localBookList);

    // 反序列化根书籍目录结构对象
    File rootBookFile = new File(GlobalDataCacheForMemorySingleton.getInstance.getApplication().getFilesDir().getPath() + "/" + CacheDataNameForSaveToFile.RootBookFileDirectory.name());
    BookFile rootDirectory = null;
    if (rootBookFile.exists()) {
      FileInputStream fileInputStream = null;
      try {
        fileInputStream = new FileInputStream(rootBookFile);
      } catch (FileNotFoundException e) {
        e.printStackTrace();
        fileInputStream = null;
      }

      rootDirectory = (BookFile) deserializeObjectFromFile(fileInputStream);

      // 删除错误的BookFile
      deleteWrongFile(rootDirectory, localBookList);
    } else {
      rootDirectory = BookFile.createDirectoryWithName(CacheDataNameForSaveToFile.RootBookFileDirectory.name(), new ArrayList<BookFile>());
      for (Book bookItem : localBookList.getShallowCopyBookList()) {
        BookFile bookFile = BookFile.createFileWithContentID(bookItem.getInfo().getContent_id());
        rootDirectory.getListFiles().add(bookFile);
      }
      saveBookFileToFileSystem(rootDirectory);
    }
    GlobalDataCacheForMemorySingleton.getInstance.setRootDirectory(rootDirectory);
  }

  // 递归判断每个BookFile是否都有与之对应的Book对象存在,若没有则果断删之
  private static void deleteWrongFile(BookFile directory, BookList localBookList) {
    List<BookFile> reomveItems = new ArrayList<BookFile>();
    List<BookFile> rootBookFiles = directory.getListFiles();
    for (BookFile bookFileItem : rootBookFiles) {
      // 文件夹分支,除了递归调用本方法查出错误BookFile对象外,还要在之后判断是否文件夹内已无书籍,如果是则将自己也添加至删除队列内
      if (bookFileItem.getContentID() == null) {
        deleteWrongFile(bookFileItem, localBookList);

        if (bookFileItem.getListFiles().size() == 0) {
          reomveItems.add(bookFileItem);
        }
      } else {
        // 书籍分支,如果无与之对应的Book对象存在,则将自己添加至删除队列内
        if (localBookList.bookByContentID(bookFileItem.getContentID()) == null) {
          reomveItems.add(bookFileItem);
        }
      }
    }
    if (reomveItems.size() > 0) {
      for (BookFile removeBookFile : reomveItems) {
        rootBookFiles.remove(removeBookFile);
      }
    }
  }

  /**
   * 从SD卡共享目录中读取书籍文件, 这里用于支持直接通过 SD卡 共享目录直接拖拽书籍压缩包到app中这个功能.
   */
  public static void readBooksFromSharedDirectory() {
    // 检索共享目录下的所有文件
    File sharedDirectoryInSDCard = LocalCacheDataPathConstant.sharedDirectoryPathInSDCard();
    // 循环检索目录下的文件结尾时候是zip格式的
    for (File sharedDirectoryDir : sharedDirectoryInSDCard.listFiles()) {
      // 获取文件的后缀名
      String fullFileName = sharedDirectoryDir.getName();
      String suffixName = fullFileName.substring(fullFileName.lastIndexOf(".") + 1);
      String fileName = fullFileName.substring(0, fullFileName.lastIndexOf("."));
      // 如果结尾为不为zip则继续
      if (!suffixName.equals("zip")) {
        continue;
      }
      // 如果结尾是zip格式
      // 将文件的名称进行md5加密
      String contentID = MD5Util.getMD5String(fileName);

      // 根据contentID从本地书籍中取出
      Book newLocalBook = GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList().bookByContentID(contentID);
      if (newLocalBook == null) {
        // 共享目录中有本本地不存在的新书籍的索引, 那就将其添加进本地目录, 状态设成 "未安装", 让用户主动进行安装
        BookInfo info = new BookInfo(contentID, fileName, null, null, null, null, null, null, null, null, null, null);
        newLocalBook = new Book(info);
        newLocalBook.setState(BookStateEnum.kBookStateEnum_NotInstalled);
      }

      newLocalBook.setFromSharedDirectory(true);

      newLocalBook.setBookTmpZipResFilePath(LocalCacheDataPathConstant.sharedDirectoryPathInSDCard() + "/" + fullFileName);
      GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList().addBook(newLocalBook);
    }
  }

  public static void readAllCacheData() {
    // 读取App配置信息
    readAppConfingInfo();
    // 读取本地缓存的 "用户登录信息"
    readUserLoginInfoToGlobalDataCacheForMemorySingleton();
    // 读取本地缓存的书籍列表
    readLocalBookListToGlobalDataCacheForMemorySingleton();
    // 从SD卡共享目录中读取书籍文件
    // 这里添加开关变量来控制是否读取SD卡共享目录的文件
    if (FunctionDeployment.readBooksFromSharedDirectory) {
      readBooksFromSharedDirectory();
    }
    // 读取公司ID
    readCompanyidInfoToGlobalDataCacheForMemorySingleton();
  }

  // ///////////////////////////////////////////////////////////////////////////
  // ///////////////////////////////////////////////////////////////////////////
  // ///////////////////////////////////////////////////////////////////////////
  // ///////////////////////////////////////////////////////////////////////////
  // ///////////////////////////////////////////////////////////////////////////
  // ///////////////////////////////////////////////////////////////////////////
  // ///////////////////////////////////////////////////////////////////////////
  public static synchronized void writeAppConfigInfo() {
    //
    final boolean isFirstStartApp = GlobalDataCacheForMemorySingleton.getInstance.isFirstStartApp();
    SharedPreferences sharedPreferences = GlobalDataCacheForMemorySingleton.getInstance.getApplication().getSharedPreferences(GlobalConstant.APP_GLOBAL_SHARED_PREFERENCES, Context.MODE_PRIVATE);
    Editor editor = sharedPreferences.edit();
    editor.putBoolean(CacheDataNameForSaveToFile.FirstStartApp.name(), isFirstStartApp);
    editor.commit();
  }

  /**
   * 保存用户登录信息到设备文件系统中
   */
  public static void writeUserLoginInfoToFileSystem() {
    SharedPreferences sharedPreferences = GlobalDataCacheForMemorySingleton.getInstance.getApplication().getSharedPreferences(GlobalConstant.APP_GLOBAL_SHARED_PREFERENCES, Context.MODE_PRIVATE);
    Editor editor = sharedPreferences.edit();
    editor.putBoolean(CacheDataNameForSaveToFile.IsLogged.name(), GlobalDataCacheForMemorySingleton.getInstance.isLogged());
    if (GlobalDataCacheForMemorySingleton.getInstance.isLogged()) {
      editor.putString(CacheDataNameForSaveToFile.UsernameForLastSuccessfulLogon.name(), GlobalDataCacheForMemorySingleton.getInstance.getUsernameForLastSuccessfulLogon());
      editor.putString(CacheDataNameForSaveToFile.PasswordForLastSuccessfulLogon.name(), GlobalDataCacheForMemorySingleton.getInstance.getPasswordForLastSuccessfulLogon());
      serializeObjectToDeviceFileSystem(CacheDataNameForSaveToFile.LatestLogonNetRespondBean.name(), GlobalDataCacheForMemorySingleton.getInstance.getLatestLogonNetRespondBean());
    } else {
      editor.remove(CacheDataNameForSaveToFile.UsernameForLastSuccessfulLogon.name());
      editor.remove(CacheDataNameForSaveToFile.PasswordForLastSuccessfulLogon.name());
      serializeObjectToDeviceFileSystem(CacheDataNameForSaveToFile.LatestLogonNetRespondBean.name(), null);
    }

    editor.commit();
  }

  /**
   * 保存书籍列表信息到设备文件系统中
   */
  public static void writeLocalBookListToFileSystem() {

    final BookList localBookList = GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList();
    List<Book> bookList = localBookList.getShallowCopyBookList();
    for (Book localBook : bookList) {
      writeLocalBookToFile(localBook, SaveLocalBookStorageTypeEnum.SD_CARD);
    }

  }

  /**
   * 保存公司ID到设备文件系统中
   */
  public static void writeCompanyidToFileSystem() {
    SharedPreferences sharedPreferences = GlobalDataCacheForMemorySingleton.getInstance.getApplication().getSharedPreferences(GlobalConstant.APP_GLOBAL_SHARED_PREFERENCES, Context.MODE_PRIVATE);
    Editor editor = sharedPreferences.edit();
    editor.putString(CacheDataNameForSaveToFile.companyid.name(), GlobalDataCacheForMemorySingleton.getInstance.getCompanyid());
    editor.commit();
  }

  private static enum SaveLocalBookStorageTypeEnum {
    // 设备文件系统
    DEVICE_FILE_SYSTEM,
    // SD卡
    SD_CARD
  };

  private static void writeLocalBookToFile(final Book localBook, final SaveLocalBookStorageTypeEnum storageTypeEnum) {
    final BookForSerializable localBookForSerializable = new BookForSerializable();
    localBookForSerializable.setBindAccount(localBook.getBindAccount());
    localBookForSerializable.setInfo(localBook.getInfo());

    // 保存书籍到文件系统中时, 书籍的状态要限制成几个规定的状态
    Book.BookStateEnum bookStateEnum = localBook.getState();
    switch (bookStateEnum) {
      case kBookStateEnum_NotInstalled:
      case kBookStateEnum_Unziping:
        // 如果当前正在解压中, 用户按下home按键, 使app进入后台, 那么此时我们保存到文件系统中得状态不能是
        // Unziping, 应该是 NotInstalled(未安装).
        // 因为用户可能会前行关闭app. 那我们就设计当用户下一次进入app时, 显示 "未安装" 这个状态, 当用户点下按钮时,
        // 要重新安装书籍
        bookStateEnum = Book.BookStateEnum.kBookStateEnum_NotInstalled;
        break;
      case kBookStateEnum_Installed:
        bookStateEnum = Book.BookStateEnum.kBookStateEnum_Installed;
        break;
      default:
        bookStateEnum = Book.BookStateEnum.kBookStateEnum_Pause;
        break;
    }
    localBookForSerializable.setState(bookStateEnum);
    localBookForSerializable.setFromSharedDirectory(localBook.isFromSharedDirectory());
    localBookForSerializable.setBookTmpZipResFilePath(localBook.getBookTmpZipResFilePath());
    // 序列化保存到书籍目录中
    // File bookSaveDir = new File(localBook.bookSaveDirPath());
    // if (!bookSaveDir.exists()) {
    // if (!bookSaveDir.mkdir()) {
    // return;
    // }
    // }

    switch (storageTypeEnum) {
      case DEVICE_FILE_SYSTEM:
        serializeObjectToDeviceFileSystem(localBook.getInfo().getContent_id() + "." + CacheDataNameForSaveToFile.LocalBook.name(), localBookForSerializable);
        break;
      case SD_CARD:
        serializeObjectToSDCard(CacheDataNameForSaveToFile.LocalBook.name(), localBook.bookSaveDirPath(), localBookForSerializable);
      default:
        break;
    }
  }

  public static void saveLocalBookToFileSystem(final Book localBook) {
    // 异步保存书籍信息到SD中, 因为向SD中写入数据很慢
    localBookSaveBlockingQueue.add(localBook.getInfo().getContent_id());
  }

  public static void saveBookFileToFileSystem(final BookFile bookFle) {
    // 异步保存书籍文件夹到队列中
    bookFileSaveBlockingQueue.add(bookFle);
  }

  private static BlockingQueue<String> localBookSaveBlockingQueue = new LinkedBlockingQueue<String>();
  private static BlockingQueue<BookFile> bookFileSaveBlockingQueue = new LinkedBlockingQueue<BookFile>();

  // 在退出应用时调用,释放读写通道
  public static final void stopSaveLocalBookThread() {
    saveLocalBookThread.interrupt();
    saveBookFileThread.interrupt();
  }

  // 序列化BookForSerializable的线程
  private static final Thread saveLocalBookThread = new Thread(new Runnable() {

    @Override
    public void run() {
      try {
        while (true && !saveLocalBookThread.isInterrupted()) {
          String bookID = localBookSaveBlockingQueue.take();
          Book localBook = GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList().bookByContentID(bookID);
          writeLocalBookToFile(localBook, SaveLocalBookStorageTypeEnum.SD_CARD);
        }
      } catch (Exception e) {
        // BlockingQueue是阻塞式队列,我们可以调用线程的isInterrupted方法,引发异常使BlockingQueue不处于等待状态.
        e.printStackTrace();
      }
      DebugLog.e(TAG, "停止 saveLocalBookThread !");
    }
  });
  // 序列化BookFile的线程
  private static final Thread saveBookFileThread = new Thread(new Runnable() {

    @Override
    public void run() {
      try {
        while (true && !saveBookFileThread.isInterrupted()) {
          BookFile bookFile = bookFileSaveBlockingQueue.take();
          serializeObjectToDeviceFileSystem(bookFile.getDirectoryName(), bookFile);
        }
      } catch (Exception e) {
        e.printStackTrace();
      }

      DebugLog.e(TAG, "停止 saveBookFileThread !");
    }
  });
  // 静态块,启动两个进行序列化操作的线程
  static {
    saveLocalBookThread.start();
    saveBookFileThread.start();
  }

  public static void writeAllCacheData() {
    // 保存 "app 配置信息"
    writeAppConfigInfo();
    // 保存 "用户登录信息"
    writeUserLoginInfoToFileSystem();
    // 保存书籍列表
    writeLocalBookListToFileSystem();
  }

  // ///////////////////////////////////////////////////////////////////////////
  // ///////////////////////////////////////////////////////////////////////////
  // ///////////////////////////////////////////////////////////////////////////
  // ///////////////////////////////////////////////////////////////////////////
  // ///////////////////////////////////////////////////////////////////////////
  // ///////////////////////////////////////////////////////////////////////////
  // ///////////////////////////////////////////////////////////////////////////

  private static void serializeObjectToSDCard(final String fileName, final String directoryPath, final Object object) {
    File file = new File(directoryPath + "/" + fileName);
    if (file.exists()) {
      file.delete();
    }
    try {
      serializeObjectToFile(object, new FileOutputStream(file));
    } catch (FileNotFoundException e) {
      e.printStackTrace();
    }

  }

  private static void serializeObjectToDeviceFileSystem(final String fileName, final Object object) {
    File file = new File(GlobalDataCacheForMemorySingleton.getInstance.getApplication().getFilesDir() + "/" + fileName);
    if (file.exists()) {
      file.delete();
    }
    try {
      serializeObjectToFile(object, GlobalDataCacheForMemorySingleton.getInstance.getApplication().openFileOutput(fileName, Context.MODE_PRIVATE));
    } catch (FileNotFoundException e) {
      e.printStackTrace();
    }
  }

  private static void serializeObjectToFile(final Object object, final FileOutputStream fileOutputStream) {
    ObjectOutputStream objectOutputStream = null;
    try {

      do {
        if (fileOutputStream == null) {
          assert false : "入参为空!";
          break;
        }
        if (object == null) {
          break;
        }

        objectOutputStream = new ObjectOutputStream(fileOutputStream);
        objectOutputStream.writeObject(object);

      } while (false);

    } catch (Exception ex) {
      ex.printStackTrace();
    } finally {
      try {
        if (objectOutputStream != null) {
          objectOutputStream.close();
        }
      } catch (IOException e) {
        e.printStackTrace();
      }

      try {
        if (fileOutputStream != null) {
          fileOutputStream.close();
        }
      } catch (IOException e) {
        e.printStackTrace();
      }
    }

  }

  private static Object deserializeObjectFromDeviceFileSystem(final String fileName) {
    Object object = null;
    try {
      object = deserializeObjectFromFile(GlobalDataCacheForMemorySingleton.getInstance.getApplication().openFileInput(fileName));
    } catch (FileNotFoundException e) {
      e.printStackTrace();
    }
    return object;
  }

  @SuppressWarnings("unused")
  private static Object deserializeObjectFromSDCard(final String fileName, final String directoryPath) {
    Object object = null;
    try {
      object = deserializeObjectFromFile(new FileInputStream(new File(directoryPath + "/" + fileName)));
    } catch (FileNotFoundException e) {
      e.printStackTrace();
    }
    return object;
  }

  private static Object deserializeObjectFromFile(FileInputStream fileInputStream) {
    Object object = null;
    ObjectInputStream objectInputStream = null;

    try {
      objectInputStream = new ObjectInputStream(fileInputStream);
      object = objectInputStream.readObject();
    } catch (Exception ex) {
      object = null;
      ex.printStackTrace();
    } finally {
      try {
        if (objectInputStream != null) {
          objectInputStream.close();
        }
      } catch (IOException e) {
        e.printStackTrace();
      }

      try {
        if (fileInputStream != null) {
          fileInputStream.close();
        }
      } catch (IOException e) {
        e.printStackTrace();
      }
    }

    return object;
  }

}
