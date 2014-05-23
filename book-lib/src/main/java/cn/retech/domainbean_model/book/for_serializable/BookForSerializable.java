package cn.retech.domainbean_model.book.for_serializable;

import java.io.Serializable;

import cn.retech.domainbean_model.book.Book.BookStateEnum;
import cn.retech.domainbean_model.booklist_in_bookstores.BookInfo;
import cn.retech.domainbean_model.login.LoginNetRespondBean;

/*
 transient 关键字

 java语言的关键字，变量修饰符，如果用transient声明一个实例变量，当对象存储时，它的值不需要维持。
 Java的serialization提供了一种持久化对象实例的机制。当持久化对象时，可能有一个特殊的对象数据成员，
 我们不想用serialization机制来保存它。为了在一个特定对象的一个字段上关闭serialization，可以在这个字段前加上关键字transient。
 当一个对象被序列化的时候，transient型变量的值不包括在序列化的表示中，然而非transient型的变量是被包括进去的。
 */
public class BookForSerializable implements Serializable {

  /**
	 * 
	 */
  private static final long serialVersionUID = 7076347749773007818L;

  public BookForSerializable() {
  }

  // 书籍信息
  private BookInfo info;
  // 书籍当前状态
  private BookStateEnum state;
  // 和当前书籍绑定的账号
  private LoginNetRespondBean bindAccount;
  // 是否来自SD卡共享目录
  private boolean isFromSharedDirectory;
  // 书籍压缩包文件存目录
  private String bookTmpZipResFilePath;

  public BookInfo getInfo() {
    return info;
  }

  public void setInfo(BookInfo info) {
    this.info = info;
  }

  public BookStateEnum getState() {
    return state;
  }

  public void setState(BookStateEnum state) {
    this.state = state;
  }

  public LoginNetRespondBean getBindAccount() {
    return bindAccount;
  }

  public void setBindAccount(LoginNetRespondBean bindAccount) {
    this.bindAccount = bindAccount;
  }

  public boolean isFromSharedDirectory() {
    return isFromSharedDirectory;
  }

  public void setFromSharedDirectory(boolean isFromSharedDirectory) {
    this.isFromSharedDirectory = isFromSharedDirectory;
  }

  public String getBookTmpZipResFilePath() {
    return bookTmpZipResFilePath;
  }

  public void setBookTmpZipResFilePath(String bookTmpZipResFilePath) {
    this.bookTmpZipResFilePath = bookTmpZipResFilePath;
  }

  @Override
  public String toString() {
    return "BookForSerializable [info=" + info + ", state=" + state + ", bindAccount=" + bindAccount + ", isFromSharedDirectory=" + isFromSharedDirectory + ", bookTmpZipResFilePath="
        + bookTmpZipResFilePath + "]";
  }

  public static boolean isValid(BookForSerializable bookForSerializable) {
    do {
      if (bookForSerializable == null) {
        break;
      }
      if (bookForSerializable.info == null) {
        break;
      }
      if (bookForSerializable.state == null) {
        break;
      }
      if (bookForSerializable.bindAccount == null) {
        break;
      }

      return true;
    } while (false);

    return false;
  }

}
