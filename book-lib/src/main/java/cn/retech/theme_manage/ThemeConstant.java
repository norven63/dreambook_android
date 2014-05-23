package cn.retech.theme_manage;

public class ThemeConstant {
	private ThemeConstant() {

	}

	// 存储xml文件中的字段
	public static enum ThemeNode {
		// 平台(最外层)
		platform,
		// 平台名称(是platform的属性)
		name,
		// 平台的值
		Android,
		// 主题包版本
		version,
		// 公司id
		companyid,
		// 主色调
		mainColor, mainTextColor, // 主色调 文字
		mainTextColorHighlighted, // 主色调 文字高亮颜色
		mainBackgroundColor, // 主背景色调
		tabHostTextFocusColorInBookStore, // tabHost 焦点tab的文字颜色
		tabHostTextDefaultColorInBookStore, // tabHost 非焦点tab的文字颜色
		tabHostBackgroundColorInBookStore, // tabHost tab下面的背景色
		tabHostLineColorInBookStore, // tabHost tab下面的直线
		tabHostPageIndicatorColorInBookStore, // tabHost 焦点tab指示器
		logoutButtonBackgroundColorDefault, // 退出按钮 默认背景颜色
		logoutButtonBackgroundColorHighlight, // 退出按钮 高亮背景颜色
		logoTextColorInSetting, // 设置界面 logo 下面的文字颜色

		// 主背景图片
		mainBackgroundImage,

		// 书籍价格标签图片
		priceTag_6, priceTag_12, priceTag_18, priceTag_25, priceTag_30, priceTag_40, priceTag_45, priceTag_50, priceTag_60, priceTag_68, priceTag_73, priceTag_78, priceTag_88, priceTag_93, priceTag_98, priceTag_108, priceTag_113, priceTag_118, priceTag_123, priceTag_128, priceTag_138, priceTag_148, priceTag_153, priceTag_158, priceTag_163, priceTag_168, priceTag_178, priceTag_188, priceTag_193, priceTag_198, priceTag_208,

		// 文件夹cell中的, 文件数量背景图片
		bookCountBackgroundInFolderCell,
		// 文件夹cell中的, 文件下载状态GIF图片(返回的是图片的存储路径)
		downloadingGIFInFolderCell,
		// 书籍价格标签 - 免费
		priceTag_free,
		// 在 设置界面 中的 公司logo
		logoInSetting,

		// 书城界面的 刷新按钮
		refreshButtonDefaultInBookStore, refreshButtonHighlightedInBookStore,

		// 书架界面的删除按钮图片
		deleteButtonDefaultInBookShelf, deleteButtonHighlightedInBookShelf,

		// 书城界面的 搜索图标
		searchButtonDefaultInBookStoreLeft,
		// 书城界面的搜索编辑框
		searchButtonDefaultInBookStoreMiddle,
		// 搜索框右侧的取消按钮
		searchButtonDefaultInBookStoreRight, searchButtonHighlightedInBookStoreRight,
		// 书城界面的搜索按钮
		serachButtonHighlightedInBookStore, serachButtonDefaultInBookStore,
		// 在 设置界面 中的 公司logo下面的背景图片
		logoBackgroundInSetting,

		// 展开文件夹中 文件夹顶部直线 右边
		folderLineRight,
		// 展开文件夹中 文件夹顶部直线 中间
		folderLineMiddle,
		// 展开文件夹中 文件夹顶部直线 左边
		folderLineLeft,

		// 展开文件夹中 文件夹名左侧
		folderNameLeft,
		// 展开文件夹中 文件夹名中间
		folderNameMiddle,
		// 展开文件夹中 文件夹名右侧
		folderNameRight,
		// 展开文件夹中 文件夹名右侧取消
		folderNameRightCancel,

		// 书架界面的搜索控件 左侧
		shelfSearchLeft,
		// 书架界面的搜索控件 中间
		shelfSearchMiddle,
		// 书架界面的搜索控件 右侧
		shelfSearchRight,

		// 书架界面的搜索控件 左侧点击
		shelfSearchPressedLeft,
		// 书架界面的搜索控件 中间点击
		shelfSearchPressedMiddle,
		// 书架界面的搜索控件 右侧点击
		shelfSearchPressedRight,
		// 搜索界面无结果的的图片
		searchNoData,
		// 导航阴影图片 -->
		 shadowSide,
		// 导航阴影滑至最左端的图片 -->
		 shadowSideFullLeft,
		// 导航阴影滑至最右端的图片 -->
		 shadowSideFullRight,

	}

}
