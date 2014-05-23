package cn.retech.domainbean_model.theme;


public final class ThemeNetRequestBean {
  // 登录名
  private final String username;
  // 密码
  private final String password;

  public ThemeNetRequestBean(String username, String password) {
    this.username = username;
    this.password = password;
  }

  public String getUsername() {
    return username;
  }

  public String getPassword() {
    return password;
  }

  @Override
  public String toString() {
    return "ThemeNetRequestBean [username=" + username + ", password=" + password + "]";
  }

}
