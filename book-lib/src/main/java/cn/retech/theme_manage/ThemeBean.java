package cn.retech.theme_manage;

/**
 * 此类是Bean,用来存储所有主题包种的资源 包括bean的初始化也需要放到这里
 */
public final class ThemeBean {
	String mainColor;// 主色调
	String mainTextColor;// 主色调 文字

	String mainTextColorHighlighted;// 主色调 文字高亮颜色

	String mainBackgroundColor;// 主背景色调
	String tabHostTextFocusColorInBookStore;// tabHost 焦点tab的文字颜色
	String tabHostTextDefaultColorInBookStore;// tabHost 非焦点tab的文字颜色
	String tabHostBackgroundColorInBookStore;// tabHost tab下面的背景色
	String tabHostLineColorInBookStore;// tabHost tab下面的直线
	String tabHostPageIndicatorColorInBookStore;// tabHost 焦点tab指示器
	String logoutButtonBackgroundColorDefault;// 退出按钮 默认背景颜色
	String logoutButtonBackgroundColorHighlight;// 退出按钮 高亮背景颜色
	String logoTextColorInSetting;// 设置界面 logo 下面的文字颜色
	// 主背景图片
	String mainBackgroundImage;

	// 书籍价格标签图片
	String priceTag_6;
	String priceTag_12;
	String priceTag_18;
	String priceTag_25;
	String priceTag_30;
	String priceTag_40;
	String priceTag_45;
	String priceTag_50;
	String priceTag_60;
	String priceTag_68;
	String priceTag_73;
	String priceTag_78;
	String priceTag_88;
	String priceTag_93;
	String priceTag_98;
	String priceTag_108;
	String priceTag_113;
	String priceTag_118;
	String priceTag_123;
	String priceTag_128;
	String priceTag_138;
	String priceTag_148;
	String priceTag_153;
	String priceTag_158;
	String priceTag_163;
	String priceTag_168;
	String priceTag_178;
	String priceTag_188;
	String priceTag_193;
	String priceTag_198;
	String priceTag_208;

	// 文件夹cell中的, 文件数量背景图片
	String bookCountBackgroundInFolderCell;
	// 文件夹cell中的, 文件下载状态GIF图片(返回的是图片的存储路径)
	String downloadingGIFInFolderCell;
	// 书籍价格标签 - 免费
	String priceTag_free;
	// 在 设置界面 中的 公司logo
	String logoInSetting;

	// 书城界面的 刷新按钮
	String refreshButtonDefaultInBookStore;
	String refreshButtonHighlightedInBookStore;

	// 书架界面的删除按钮图片
	String deleteButtonDefaultInBookShelf;
	String deleteButtonHighlightedInBookShelf;

	// 书城界面的 搜索图标
	String searchButtonDefaultInBookStoreLeft;
	// 书城界面的搜索编辑框
	String searchButtonDefaultInBookStoreMiddle;
	// 搜索框右侧的取消按钮
	String searchButtonDefaultInBookStoreRight;
	String searchButtonHighlightedInBookStoreRight;
	// 书城界面的搜索按钮
	String serachButtonHighlightedInBookStore;
	String serachButtonDefaultInBookStore;

	// 在 设置界面 中的 公司logo下面的背景图片
	String logoBackgroundInSetting;

	// 展开文件夹中 文件夹顶部直线 右边
	String folderLineRight;

	// 展开文件夹中 文件夹顶部直线 中间
	String folderLineMiddle;
	// 展开文件夹中 文件夹顶部直线 左边
	String folderLineLeft;

	// 展开文件夹中 文件夹名左侧
	String folderNameLeft;
	// 展开文件夹中 文件夹名中间
	String folderNameMiddle;
	// 展开文件夹中 文件夹名右侧
	String folderNameRight;
	// 展开文件夹中 文件夹名右侧取消
	String folderNameRightCancel;

	// 书架界面的搜索控件 左侧
	String shelfSearchLeft;
	// 书架界面的搜索控件 中间
	String shelfSearchMiddle;
	// 书架界面的搜索控件 右侧
	String shelfSearchRight;

	// 书架界面的搜索控件 左侧点击
	String shelfSearchPressedLeft;
	// 书架界面的搜索控件 中间点击
	String shelfSearchPressedMiddle;
	// 书架界面的搜索控件 右侧点击
	String shelfSearchPressedRight;
	// 搜索界面无结果的的图片
	String searchNoData;
	// 导航阴影图片 -->
	String shadowSide;
	// 导航阴影滑至最左端的图片 -->
	String shadowSideFullLeft;
	// 导航阴影滑至最右端的图片 -->
	String shadowSideFullRight;
	/**
	 * 私有构造
	 */
	ThemeBean() {
	}

	public String getShadowSide() {
		return shadowSide;
	}

	public String getShadowSideFullLeft() {
		return shadowSideFullLeft;
	}

	public String getShadowSideFullRight() {
		return shadowSideFullRight;
	}

	public String getSearchNoData() {
		return searchNoData;
	}

	public String getSerachButtonDefaultInBookStore() {
		return serachButtonDefaultInBookStore;
	}

	public String getMainColor() {
		return mainColor;
	}

	public String getMainTextColor() {
		return mainTextColor;
	}

	public String getMainBackgroundColor() {
		return mainBackgroundColor;
	}

	public String getTabHostTextFocusColorInBookStore() {
		return tabHostTextFocusColorInBookStore;
	}

	public String getTabHostTextDefaultColorInBookStore() {
		return tabHostTextDefaultColorInBookStore;
	}

	public String getTabHostBackgroundColorInBookStore() {
		return tabHostBackgroundColorInBookStore;
	}

	public String getTabHostLineColorInBookStore() {
		return tabHostLineColorInBookStore;
	}

	public String getTabHostPageIndicatorColorInBookStore() {
		return tabHostPageIndicatorColorInBookStore;
	}

	public String getLogoutButtonBackgroundColorDefault() {
		return logoutButtonBackgroundColorDefault;
	}

	public String getLogoutButtonBackgroundColorHighlight() {
		return logoutButtonBackgroundColorHighlight;
	}

	public String getLogoTextColorInSetting() {
		return logoTextColorInSetting;
	}

	public String getMainBackgroundImage() {
		return mainBackgroundImage;
	}

	public String getPriceTag_6() {
		return priceTag_6;
	}

	public String getPriceTag_12() {
		return priceTag_12;
	}

	public String getPriceTag_18() {
		return priceTag_18;
	}

	public String getPriceTag_25() {
		return priceTag_25;
	}

	public String getPriceTag_30() {
		return priceTag_30;
	}

	public String getPriceTag_40() {
		return priceTag_40;
	}

	public String getPriceTag_45() {
		return priceTag_45;
	}

	public String getPriceTag_50() {
		return priceTag_50;
	}

	public String getPriceTag_60() {
		return priceTag_60;
	}

	public String getPriceTag_68() {
		return priceTag_68;
	}

	public String getPriceTag_73() {
		return priceTag_73;
	}

	public String getPriceTag_78() {
		return priceTag_78;
	}

	public String getPriceTag_88() {
		return priceTag_88;
	}

	public String getPriceTag_93() {
		return priceTag_93;
	}

	public String getPriceTag_98() {
		return priceTag_98;
	}

	public String getPriceTag_108() {
		return priceTag_108;
	}

	public String getPriceTag_113() {
		return priceTag_113;
	}

	public String getPriceTag_118() {
		return priceTag_118;
	}

	public String getPriceTag_123() {
		return priceTag_123;
	}

	public String getPriceTag_128() {
		return priceTag_128;
	}

	public String getPriceTag_138() {
		return priceTag_138;
	}

	public String getPriceTag_148() {
		return priceTag_148;
	}

	public String getPriceTag_153() {
		return priceTag_153;
	}

	public String getPriceTag_158() {
		return priceTag_158;
	}

	public String getPriceTag_163() {
		return priceTag_163;
	}

	public String getPriceTag_168() {
		return priceTag_168;
	}

	public String getPriceTag_178() {
		return priceTag_178;
	}

	public String getPriceTag_188() {
		return priceTag_188;
	}

	public String getPriceTag_193() {
		return priceTag_193;
	}

	public String getPriceTag_198() {
		return priceTag_198;
	}

	public String getPriceTag_208() {
		return priceTag_208;
	}

	public String getBookCountBackgroundInFolderCell() {
		return bookCountBackgroundInFolderCell;
	}

	public String getDownloadingGIFInFolderCell() {
		return downloadingGIFInFolderCell;
	}

	public String getPriceTag_free() {
		return priceTag_free;
	}

	public String getLogoInSetting() {
		return logoInSetting;
	}

	public String getRefreshButtonDefaultInBookStore() {
		return refreshButtonDefaultInBookStore;
	}

	public String getRefreshButtonHighlightedInBookStore() {
		return refreshButtonHighlightedInBookStore;
	}

	public String getDeleteButtonDefaultInBookShelf() {
		return deleteButtonDefaultInBookShelf;
	}

	public String getDeleteButtonHighlightedInBookShelf() {
		return deleteButtonHighlightedInBookShelf;
	}

	public String getSearchButtonDefaultInBookStoreLeft() {
		return searchButtonDefaultInBookStoreLeft;
	}

	public String getSearchButtonDefaultInBookStoreMiddle() {
		return searchButtonDefaultInBookStoreMiddle;
	}

	public String getSearchButtonDefaultInBookStoreRight() {
		return searchButtonDefaultInBookStoreRight;
	}

	public String getSearchButtonHighlightedInBookStoreRight() {
		return searchButtonHighlightedInBookStoreRight;
	}

	public String getLogoBackgroundInSetting() {
		return logoBackgroundInSetting;
	}

	public String getSerachButtonHighlightedInBookStore() {
		return serachButtonHighlightedInBookStore;
	}

	public String getMainTextColorHighlighted() {
		return mainTextColorHighlighted;
	}

	public String getFolderLineRight() {
		return folderLineRight;
	}

	public String getFolderLineMiddle() {
		return folderLineMiddle;
	}

	public String getFolderLineLeft() {
		return folderLineLeft;
	}

	public String getFolderNameLeft() {
		return folderNameLeft;
	}

	public String getFolderNameMiddle() {
		return folderNameMiddle;
	}

	public String getFolderNameRight() {
		return folderNameRight;
	}

	public String getFolderNameRightCancel() {
		return folderNameRightCancel;
	}

	public String getShelfSearchLeft() {
		return shelfSearchLeft;
	}

	public String getShelfSearchMiddle() {
		return shelfSearchMiddle;
	}

	public String getShelfSearchRight() {
		return shelfSearchRight;
	}

	public String getShelfSearchPressedLeft() {
		return shelfSearchPressedLeft;
	}

	public String getShelfSearchPressedMiddle() {
		return shelfSearchPressedMiddle;
	}

	public String getShelfSearchPressedRight() {
		return shelfSearchPressedRight;
	}

	@Override
	public String toString() {
		return "ThemeBean [mainColor=" + mainColor + ", mainTextColor=" + mainTextColor + ", mainTextColorHighlighted=" + mainTextColorHighlighted + ", mainBackgroundColor=" + mainBackgroundColor
				+ ", tabHostTextFocusColorInBookStore=" + tabHostTextFocusColorInBookStore + ", tabHostTextDefaultColorInBookStore=" + tabHostTextDefaultColorInBookStore
				+ ", tabHostBackgroundColorInBookStore=" + tabHostBackgroundColorInBookStore + ", tabHostLineColorInBookStore=" + tabHostLineColorInBookStore
				+ ", tabHostPageIndicatorColorInBookStore=" + tabHostPageIndicatorColorInBookStore + ", logoutButtonBackgroundColorDefault=" + logoutButtonBackgroundColorDefault
				+ ", logoutButtonBackgroundColorHighlight=" + logoutButtonBackgroundColorHighlight + ", logoTextColorInSetting=" + logoTextColorInSetting + ", mainBackgroundImage="
				+ mainBackgroundImage + ", priceTag_6=" + priceTag_6 + ", priceTag_12=" + priceTag_12 + ", priceTag_18=" + priceTag_18 + ", priceTag_25=" + priceTag_25 + ", priceTag_30="
				+ priceTag_30 + ", priceTag_40=" + priceTag_40 + ", priceTag_45=" + priceTag_45 + ", priceTag_50=" + priceTag_50 + ", priceTag_60=" + priceTag_60 + ", priceTag_68=" + priceTag_68
				+ ", priceTag_73=" + priceTag_73 + ", priceTag_78=" + priceTag_78 + ", priceTag_88=" + priceTag_88 + ", priceTag_93=" + priceTag_93 + ", priceTag_98=" + priceTag_98
				+ ", priceTag_108=" + priceTag_108 + ", priceTag_113=" + priceTag_113 + ", priceTag_118=" + priceTag_118 + ", priceTag_123=" + priceTag_123 + ", priceTag_128=" + priceTag_128
				+ ", priceTag_138=" + priceTag_138 + ", priceTag_148=" + priceTag_148 + ", priceTag_153=" + priceTag_153 + ", priceTag_158=" + priceTag_158 + ", priceTag_163=" + priceTag_163
				+ ", priceTag_168=" + priceTag_168 + ", priceTag_178=" + priceTag_178 + ", priceTag_188=" + priceTag_188 + ", priceTag_193=" + priceTag_193 + ", priceTag_198=" + priceTag_198
				+ ", priceTag_208=" + priceTag_208 + ", bookCountBackgroundInFolderCell=" + bookCountBackgroundInFolderCell + ", downloadingGIFInFolderCell=" + downloadingGIFInFolderCell
				+ ", priceTag_free=" + priceTag_free + ", logoInSetting=" + logoInSetting + ", refreshButtonDefaultInBookStore=" + refreshButtonDefaultInBookStore
				+ ", refreshButtonHighlightedInBookStore=" + refreshButtonHighlightedInBookStore + ", deleteButtonDefaultInBookShelf=" + deleteButtonDefaultInBookShelf
				+ ", deleteButtonHighlightedInBookShelf=" + deleteButtonHighlightedInBookShelf + ", searchButtonDefaultInBookStoreLeft=" + searchButtonDefaultInBookStoreLeft
				+ ", searchButtonDefaultInBookStoreMiddle=" + searchButtonDefaultInBookStoreMiddle + ", searchButtonDefaultInBookStoreRight=" + searchButtonDefaultInBookStoreRight
				+ ", searchButtonHighlightedInBookStoreRight=" + searchButtonHighlightedInBookStoreRight + ", serachButtonHighlightedInBookStore=" + serachButtonHighlightedInBookStore
				+ ", serachButtonDefaultInBookStore=" + serachButtonDefaultInBookStore + ", logoBackgroundInSetting=" + logoBackgroundInSetting + ", folderLineRight=" + folderLineRight
				+ ", folderLineMiddle=" + folderLineMiddle + ", folderLineLeft=" + folderLineLeft + ", folderNameLeft=" + folderNameLeft + ", folderNameMiddle=" + folderNameMiddle
				+ ", folderNameRight=" + folderNameRight + ", folderNameRightCancel=" + folderNameRightCancel + ", shelfSearchLeft=" + shelfSearchLeft + ", shelfSearchMiddle=" + shelfSearchMiddle
				+ ", shelfSearchRight=" + shelfSearchRight + ", shelfSearchPressedLeft=" + shelfSearchPressedLeft + ", shelfSearchPressedMiddle=" + shelfSearchPressedMiddle
				+ ", shelfSearchPressedRight=" + shelfSearchPressedRight + ", searchNoData=" + searchNoData + ", shadowSide=" + shadowSide + ", shadowSideFullLeft=" + shadowSideFullLeft
				+ ", shadowSideFullRight=" + shadowSideFullRight + "]";
	}

	

}
