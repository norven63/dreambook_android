package cn.retech.global_data_cache;

import android.app.Application;
import android.text.TextUtils;
import cn.retech.domainbean_model.book.BookFile;
import cn.retech.domainbean_model.book.BookList;
import cn.retech.domainbean_model.login.LoginNetRespondBean;

/**
 * 需要全局缓存的数据
 * 
 */
public enum GlobalDataCacheForMemorySingleton {
  getInstance;

  // 因为在lib包中, 没有Application对象, 所以要求具体项目在MyApplication 中设置当前 application 属性
  private Application application;

  public Application getApplication() {
    return application;
  }

  public void setApplication(Application application) {
    if (this.application != null) {
      // 只给用户设置application属性一次的机会, 这样用户就无法随意的修改application属性了.
      return;
    }
    this.application = application;
  }

  // 是否是第一次启动App的标志位
  private boolean isFirstStartApp;

  public boolean isFirstStartApp() {
    return isFirstStartApp;
  }

  public void setFirstStartApp(boolean isFirstStartApp) {
    this.isFirstStartApp = isFirstStartApp;
  }

  // 是否需要在app启动时, 显示 "初学者指南界面"
  private boolean isNeedShowBeginnerGuide;

  public boolean isNeedShowBeginnerGuide() {
    return isNeedShowBeginnerGuide;
  }

  public void setNeedShowBeginnerGuide(boolean isNeedShowBeginnerGuide) {
    this.isNeedShowBeginnerGuide = isNeedShowBeginnerGuide;
  }

  // 是否需要自动登录的标志
  private boolean isNeedAutologin;

  public boolean isNeedAutologin() {
    return isNeedAutologin;
  }

  public void setNeedAutologin(boolean isNeedAutologin) {
    this.isNeedAutologin = isNeedAutologin;
  }

  /**
   * 用户是否登录的标志
   */
  private boolean isLogged;

  public boolean isLogged() {
    return isLogged;
  }

  // 用户最后一次登录成功时的用户名/密码(企业账户/公共账户 登录成功都会保存在这里)
  private String usernameForLastSuccessfulLogon;

  public String getUsernameForLastSuccessfulLogon() {
    return usernameForLastSuccessfulLogon;
  }

  private String passwordForLastSuccessfulLogon;

  public String getPasswordForLastSuccessfulLogon() {
    return passwordForLastSuccessfulLogon;
  }

  // 用户最后一次登录成功返回的用户信息
  private LoginNetRespondBean latestLogonNetRespondBean;

  public LoginNetRespondBean getLatestLogonNetRespondBean() {
    return latestLogonNetRespondBean;
  }

  /**
   * 记录用户登录成功后的重要信息
   * 
   * @param logonNetRespondBean
   * @param usernameForLastSuccessfulLogon
   * @param passwordForLastSuccessfulLogon
   */
  public void noteSignInSuccessfulInfo(LoginNetRespondBean latestLogonNetRespondBean, String usernameForLastSuccessfulLogon, String passwordForLastSuccessfulLogon) {

    if (latestLogonNetRespondBean == null) {
      throw new IllegalArgumentException("logonNetRespondBean is null !");
    }

    if (TextUtils.isEmpty(usernameForLastSuccessfulLogon) || TextUtils.isEmpty(passwordForLastSuccessfulLogon)) {
      throw new IllegalArgumentException("username or password is empty ! ");
    }
    // 用户登录了
    this.isLogged = true;
    // 保存用户最后一次登录成功时的 用户名 / 密码
    this.usernameForLastSuccessfulLogon = usernameForLastSuccessfulLogon;
    this.passwordForLastSuccessfulLogon = passwordForLastSuccessfulLogon;
    // 保存用户最后一次登录之后的信息Bean
    this.latestLogonNetRespondBean = latestLogonNetRespondBean;
    // 必须手动设置当前用户的登录密码到 LogonNetRespondBean 中, 因为服务器不会给我们返回这个字段
    this.latestLogonNetRespondBean.setPassword(passwordForLastSuccessfulLogon);
    // 保存到文件系统中
    GlobalDataCacheForNeedSaveToFileSystem.writeUserLoginInfoToFileSystem();
  }

  public void noteSignOutSuccessfulInfo() {
    // 用户登出了
    this.isLogged = false;
    // 保留用户最后一次登录成功时的 用户名 / 密码
    this.usernameForLastSuccessfulLogon = null;
    this.passwordForLastSuccessfulLogon = null;
    this.latestLogonNetRespondBean = null;
    // 保存到文件系统中
    GlobalDataCacheForNeedSaveToFileSystem.writeUserLoginInfoToFileSystem();
  }

  // 本地书籍列表(所谓 "本地" 是指在 "书架界面显示的书籍" 也就是说, 需要固化到文件系统中的书籍列表, 跟它对应的是 "书城界面的书籍列表",
  // 这个就是临时存在的.
  private BookList localBookList;

  public BookList getLocalBookList() {
    return localBookList;
  }

  public void setLocalBookList(BookList localBookList) {
    this.localBookList = localBookList;
  }

  // 书籍虚拟文件夹的根目录(真实的模型是localBookList, 而rootDirectory是虚拟文件夹, 要依附于localBookList)
  private BookFile rootDirectory;

  public BookFile getRootDirectory() {
    return rootDirectory;
  }

  public void setRootDirectory(BookFile rootDirectory) {
    this.rootDirectory = rootDirectory;
  }

  // 从配置文件中获取的渠道名称
  private String channelName;

  public String getChannelName() {
    return channelName;
  }

  public void setChannelName(String channelName) {
    this.channelName = channelName;
  }

  // 从配置文件中获取的版本名称
  private String versionName;

  public String getVersionName() {
    return versionName;
  }

  public void setVersionName(String versionName) {
    this.versionName = versionName;
  }

  // 根据账户名相对的公司id,作为主题包的路径
  private String companyid;

  public String getCompanyid() {

    return companyid;
  }

  public void setCompanyid(String companyid) {
    this.companyid = companyid;
  }

}
