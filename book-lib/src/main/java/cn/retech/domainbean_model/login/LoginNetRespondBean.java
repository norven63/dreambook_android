package cn.retech.domainbean_model.login;

import java.io.Serializable;

public final class LoginNetRespondBean implements Serializable {
  /**
   * 
   */
  private static final long serialVersionUID = 2898594807600560964L;

  private String user;// 登录用户名
  private String password;// 登录密码
  private String companyName;// 公司名字
  private String subscriptionCode;// 隶属公司ID
  private String name;// 用户名
  private String nameKana;// 昵称
  private String email;// 邮箱

  @Override
  public LoginNetRespondBean clone() {
    LoginNetRespondBean cloneObject = null;
    try {
      cloneObject = (LoginNetRespondBean) super.clone();
      cloneObject.user = this.user;
      cloneObject.password = this.password;
      cloneObject.companyName = this.companyName;
      cloneObject.subscriptionCode = this.subscriptionCode;
      cloneObject.name = this.name;
      cloneObject.nameKana = this.nameKana;
      cloneObject.email = this.email;

    } catch (CloneNotSupportedException e) {
      throw new RuntimeException(e); // won't happen        
    }
    return cloneObject;
  }

  public String getUser() {
    return user;
  }

  public String getPassword() {
    return password;
  }

  public String getCompanyName() {
    return companyName;
  }

  public String getName() {
    return name;
  }

  public String getNameKana() {
    return nameKana;
  }

  public String getEmail() {
    return email;
  }

  public static long getSerialversionuid() {
    return serialVersionUID;
  }

  public void setUser(String user) {
    this.user = user;
  }

  public void setPassword(String password) {
    this.password = password;
  }

  public void setCompanyName(String companyName) {
    this.companyName = companyName;
  }

  public void setName(String name) {
    this.name = name;
  }

  public void setNameKana(String nameKana) {
    this.nameKana = nameKana;
  }

  public void setEmail(String email) {
    this.email = email;
  }

  public String getSubscriptionCode() {
    return subscriptionCode;
  }

  public void setSubscriptionCode(String subscriptionCode) {
    this.subscriptionCode = subscriptionCode;
  }

  @Override
  public String toString() {
    return "LogonNetRespondBean [user=" + user + ", password=" + password + ", companyName=" + companyName + ", subscriptionCode=" + subscriptionCode + ", name=" + name + ", nameKana=" + nameKana
        + ", email=" + email + "]";
  }

}
