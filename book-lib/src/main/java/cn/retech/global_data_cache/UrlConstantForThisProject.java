package cn.retech.global_data_cache;

public final class UrlConstantForThisProject {
  private UrlConstantForThisProject() {

  }

  // 外网地址
  public static final String kUrlConstant_MainUrl = "https://dreambook.retechcorp.com";
  // 主Path
  public static final String kUrlConstant_MainPtah = "dreambook";

  // 1 获取书籍分类
  public static final String kUrlConstant_SpecialPath_book_categories = "categories";
  // 2 用户登录
  public static final String kUrlConstant_SpecialPath_login = "account/login";
  // 3 获取要下载的书籍的URL
  public static final String kUrlConstant_SpecialPath_book_downlaod_url = "content/download/";
  // 4 企业书库的书籍列表
  public static final String kUrlConstant_SpecialPath_book_list_in_bookstores = "content/list";
  // 5 主题
  public static final String kUrlConstant_SpecialPath_theme = "theme/index";
  // 6 书籍搜索(联网)
  public static final String kUrlConstant_SpecialPath_book_query = "content/query";
  // 7 检测版本升级信息
  public static final String kUrlConstant_SpecialPath_latest_app_version_info = "download/update_config.xml";
}
