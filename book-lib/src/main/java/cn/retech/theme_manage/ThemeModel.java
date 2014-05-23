package cn.retech.theme_manage;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserFactory;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.util.DisplayMetrics;
import android.view.View;
import cn.retech.domainbean_model.theme.ThemeNetRespondBean;
import cn.retech.toolutils.DebugLog;

public class ThemeModel {
	// 主题信息 包含主题版本,所属公司,下载url
	private ThemeNetRespondBean themeInfo;
	private final ThemeBean themeBean = new ThemeBean();

	public ThemeNetRespondBean getThemeInfo() {
		return themeInfo;
	}

	public ThemeBean getThemeBean() {
		return themeBean;
	}

	private ThemeModel() {

	}

	private static String getConfigXMLPATH(String filePath) {
		return filePath + "/theme_config.xml";
	}

	public static class ThemeBuilder {
		private static String parentPathString = "";

		/**
		 * 
		 * @param themeInfo
		 *            主题包信息,包含主题版本,所属公司,下载URL
		 * @param filePath
		 *            文件存储路径
		 * @return
		 */
		public ThemeModel buildTheme(ThemeNetRespondBean themeInfo, String filePath) {
			DebugLog.i(">>>>>>>>>>>>>>>>>>>替换主题<<<<<<<<<<<<<<<<<<<", themeInfo.toString() + ">>>>>>>>>>>>>>>>>>>替换主题<<<<<<<<<<<<<<<<<<<" + filePath);
			ThemeBuilder.parentPathString = filePath;
			// 创建model
			ThemeModel themeModel = new ThemeModel();
			themeModel.themeInfo = themeInfo;
			// 读取xml文件,xml文件中包含所有色值,读取并设置
			File xmlFile = new File(getConfigXMLPATH(filePath));

			if (!xmlFile.exists()) {
				return null;
			}
			// 读取xml文件中的色值
			try {
				// 将文件转为string然后转为byte并创建输入流
				InputStream inputStreamOfRespondData = new FileInputStream(xmlFile);

				XmlPullParserFactory parserCreator = XmlPullParserFactory.newInstance();
				XmlPullParser parser = parserCreator.newPullParser();
				parser.setInput(inputStreamOfRespondData, "utf-8");
				int parserEvent = parser.getEventType();
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
								// 公司ID
								if (!parser.getAttributeValue(null, ThemeConstant.ThemeNode.companyid.name()).equals(themeModel.themeInfo.getCompanyid())) {
									break;
								}
								// 主题版本
								if (!parser.getAttributeValue(null, ThemeConstant.ThemeNode.version.name()).equals(themeModel.themeInfo.getVersion())) {
									break;
								}
							} while (false);
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.mainColor.name())) {// 主色调
							themeModel.themeBean.mainColor = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.mainTextColor.name())) {// 主色调文字
							themeModel.themeBean.mainTextColor = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.mainBackgroundColor.name())) {// 主背景色调
							themeModel.themeBean.mainBackgroundColor = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.mainTextColorHighlighted.name())) {// 主色调
																																// 文字高亮颜色
							themeModel.themeBean.mainTextColorHighlighted = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.tabHostTextFocusColorInBookStore.name())) {// tabHost焦点tab的文字颜色
							themeModel.themeBean.tabHostTextFocusColorInBookStore = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.tabHostTextDefaultColorInBookStore.name())) {// tabHost非焦点tab的文字颜色
							themeModel.themeBean.tabHostTextDefaultColorInBookStore = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.tabHostBackgroundColorInBookStore.name())) {// tabHosttab下面的背景色
							themeModel.themeBean.tabHostBackgroundColorInBookStore = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.tabHostLineColorInBookStore.name())) {// tabHosttab下面的直线
							themeModel.themeBean.tabHostLineColorInBookStore = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.tabHostPageIndicatorColorInBookStore.name())) {// tabHost焦点tab指示器
							themeModel.themeBean.tabHostPageIndicatorColorInBookStore = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.logoutButtonBackgroundColorDefault.name())) { // 退出按钮默认背景颜色
							themeModel.themeBean.logoutButtonBackgroundColorDefault = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.logoutButtonBackgroundColorHighlight.name())) {// 退出按钮高亮背景颜色
							themeModel.themeBean.logoutButtonBackgroundColorHighlight = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.logoTextColorInSetting.name())) { // 设置界面logo下面的文字颜色
							themeModel.themeBean.logoTextColorInSetting = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.mainBackgroundImage.name())) {// 主背景图片
							themeModel.themeBean.mainBackgroundImage = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_6.name())) {// 书籍价格标签图片
							themeModel.themeBean.priceTag_6 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_12.name())) {
							themeModel.themeBean.priceTag_12 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_18.name())) {
							themeModel.themeBean.priceTag_18 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_25.name())) {
							themeModel.themeBean.priceTag_25 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_30.name())) {
							themeModel.themeBean.priceTag_30 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_40.name())) {
							themeModel.themeBean.priceTag_40 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_45.name())) {
							themeModel.themeBean.priceTag_45 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_50.name())) {
							themeModel.themeBean.priceTag_50 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_60.name())) {
							themeModel.themeBean.priceTag_60 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_68.name())) {
							themeModel.themeBean.priceTag_68 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_73.name())) {
							themeModel.themeBean.priceTag_73 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_78.name())) {
							themeModel.themeBean.priceTag_78 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_88.name())) {
							themeModel.themeBean.priceTag_88 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_93.name())) {
							themeModel.themeBean.priceTag_93 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_98.name())) {
							themeModel.themeBean.priceTag_98 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_108.name())) {
							themeModel.themeBean.priceTag_108 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_113.name())) {
							themeModel.themeBean.priceTag_113 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_118.name())) {
							themeModel.themeBean.priceTag_118 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_123.name())) {
							themeModel.themeBean.priceTag_123 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_128.name())) {
							themeModel.themeBean.priceTag_128 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_138.name())) {
							themeModel.themeBean.priceTag_138 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_148.name())) {
							themeModel.themeBean.priceTag_148 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_153.name())) {
							themeModel.themeBean.priceTag_153 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_158.name())) {
							themeModel.themeBean.priceTag_158 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_163.name())) {
							themeModel.themeBean.priceTag_163 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_168.name())) {
							themeModel.themeBean.priceTag_168 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_178.name())) {
							themeModel.themeBean.priceTag_178 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_188.name())) {
							themeModel.themeBean.priceTag_188 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_193.name())) {
							themeModel.themeBean.priceTag_193 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_198.name())) {
							themeModel.themeBean.priceTag_198 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_208.name())) {
							themeModel.themeBean.priceTag_208 = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.bookCountBackgroundInFolderCell.name())) {// 文件夹cell中的,文件数量背景图片
							themeModel.themeBean.bookCountBackgroundInFolderCell = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.downloadingGIFInFolderCell.name())) {// 文件下载状态GIF图片
							themeModel.themeBean.downloadingGIFInFolderCell = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.priceTag_free.name())) {// 书籍价格标签免费
							themeModel.themeBean.priceTag_free = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.logoInSetting.name())) {// 在设置界面中的公司logo
							themeModel.themeBean.logoInSetting = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.refreshButtonDefaultInBookStore.name())) {// 书城界面的刷新按钮
							themeModel.themeBean.refreshButtonDefaultInBookStore = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.refreshButtonHighlightedInBookStore.name())) {// 书城界面的刷新按钮
							themeModel.themeBean.refreshButtonHighlightedInBookStore = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.deleteButtonDefaultInBookShelf.name())) {// 书架界面的删除按钮图片
							themeModel.themeBean.deleteButtonDefaultInBookShelf = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.deleteButtonHighlightedInBookShelf.name())) {// 书架界面的删除按钮图片
							themeModel.themeBean.deleteButtonHighlightedInBookShelf = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.searchButtonDefaultInBookStoreLeft.name())) {// 书城界面的
																																			// 搜索图标
							themeModel.themeBean.searchButtonDefaultInBookStoreLeft = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.searchButtonDefaultInBookStoreMiddle.name())) {// 书城界面的搜索编辑框
							themeModel.themeBean.searchButtonDefaultInBookStoreMiddle = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.searchButtonDefaultInBookStoreRight.name())) {// 搜索框右侧的取消按钮
							themeModel.themeBean.searchButtonDefaultInBookStoreRight = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.searchButtonHighlightedInBookStoreRight.name())) {// 搜索框右侧的取消按钮
							themeModel.themeBean.searchButtonHighlightedInBookStoreRight = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.serachButtonHighlightedInBookStore.name())) {// 书城界面的搜索按钮
							themeModel.themeBean.serachButtonHighlightedInBookStore = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.serachButtonDefaultInBookStore.name())) {// 书城界面的搜索按钮
							themeModel.themeBean.serachButtonDefaultInBookStore = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.logoBackgroundInSetting.name())) {// 在设置界面中的公司logo背景图片
							themeModel.themeBean.logoBackgroundInSetting = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.folderLineRight.name())) {// 展开文件夹中
																														// 文件夹顶部直线
																														// 右边
							themeModel.themeBean.folderLineRight = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.folderLineMiddle.name())) {// 展开文件夹中
																														// 文件夹顶部直线
																														// 中间
							themeModel.themeBean.folderLineMiddle = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.folderLineLeft.name())) {// 展开文件夹中
																														// 文件夹顶部直线
																														// 左边
							themeModel.themeBean.folderLineLeft = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.folderNameLeft.name())) {// 展开文件夹中
																														// 文件夹名左侧
							themeModel.themeBean.folderNameLeft = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.folderNameMiddle.name())) {// 展开文件夹中
																														// 文件夹名中间
							themeModel.themeBean.folderNameMiddle = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.folderNameRight.name())) {// 展开文件夹中
																														// 文件夹名右侧
							themeModel.themeBean.folderNameRight = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.folderNameRightCancel.name())) {// 展开文件夹中
																																// 文件夹名右侧取消
							themeModel.themeBean.folderNameRightCancel = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.shelfSearchLeft.name())) {// 书架界面的搜索控件
																														// 左侧
							themeModel.themeBean.shelfSearchLeft = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.shelfSearchMiddle.name())) {// 书架界面的搜索控件
																															// 中间
							themeModel.themeBean.shelfSearchMiddle = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.shelfSearchRight.name())) {// 书架界面的搜索控件
																														// 右侧
							themeModel.themeBean.shelfSearchRight = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.shelfSearchPressedLeft.name())) {// 书架界面的搜索控件
																																// 左侧点击
							themeModel.themeBean.shelfSearchPressedLeft = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.shelfSearchPressedMiddle.name())) {// 书架界面的搜索控件
																																// 中间点击
							themeModel.themeBean.shelfSearchPressedMiddle = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.shelfSearchPressedRight.name())) {// 书架界面的搜索控件
																																// 右侧点击
							themeModel.themeBean.shelfSearchPressedRight = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.searchNoData.name())) {// 搜索界面无结果的的图片
							themeModel.themeBean.searchNoData = parser.nextText();
						}

						else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.shadowSide.name())) {// 导航阴影图片
							themeModel.themeBean.shadowSide = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.shadowSideFullLeft.name())) {// 导航阴影滑至最左端的图片
							themeModel.themeBean.shadowSideFullLeft = parser.nextText();
						} else if (parser.getName().equalsIgnoreCase(ThemeConstant.ThemeNode.shadowSideFullRight.name())) {// 导航阴影滑至最右端的图片
							themeModel.themeBean.shadowSideFullRight = parser.nextText();
						}
						break;

					case XmlPullParser.END_TAG:// 结束元素事件
						break;
					}

					parserEvent = parser.next();
				}
				inputStreamOfRespondData.close();
				DebugLog.i("=============", ">>>>>>>>>>>>>>>>>>>替换主题完成<<<<<<<<<<<<<<<<<<<");
			} catch (Exception e) {
				e.printStackTrace();
			}

			return themeModel;
		}

		public List<File> ReadAllFile(String filePath) {
			File f = null;
			f = new File(filePath);
			File[] files = f.listFiles(); // 得到f文件夹下面的所有文件。
			List<File> list = new ArrayList<File>();
			for (File file : files) {
				if (file.isDirectory()) {
					// 如确定当前路径是文件夹，则循环读取这个文件夹下的所有文件
					ReadAllFile(file.getAbsolutePath());
				} else {
					list.add(file);
				}
			}
			return list;
		}

		public void ReadFile(String filePath) {
			File f = null;
			f = new File(filePath);
			File[] files = f.listFiles(); // 得到f文件夹下面的所有文件。
			List<File> list = new ArrayList<File>();
			for (File file : files) {
				list.add(file);
			}
			for (File file : files) {
				System.out.println(file.getAbsolutePath());
			}
		}

		/**
		 * 文件夹所对应的屏幕密度
		 * 
		 * drawable-hdpi 对应DPI为240
		 * 
		 * drawable-ldpi 对应DPI为120
		 * 
		 * drawable-mdpi 对应DPI为160
		 * 
		 * drawable-xhdpi 对应DPI为320
		 * 
		 * drawable-xxhdpi 对应DPI为480
		 * 
		 */
		private enum FolderCorrespondingScreenDensityEnum {
			DRAWABLE_HDPI(240, "drawable-hdpi"), DRAWABLE_LDPI(120, "drawable-ldpi"), DRAWABLE_MDPI(160, "drawable-mdpi"), DRAWABLE_XHDPI(320, "drawable-xhdpi"), DRAWABLE_XXHDPI(480,
					"drawable-xxhdpi");
			private final int screenDensityValue;
			private final String folderName;

			FolderCorrespondingScreenDensityEnum(int screenDensityValue, String folderName) {
				this.screenDensityValue = screenDensityValue;
				this.folderName = folderName;
			}

			public String getFolderName() {
				return folderName;
			}

			public int getScreenDensityValue() {
				return screenDensityValue;
			}

		}

		/**
		 * 根据屏幕密度返回路径
		 */
		public static String getPathAccordingScreenDensity(Context context) {
			DisplayMetrics dm = new DisplayMetrics();
			dm = context.getResources().getDisplayMetrics();
			int densityDPI = dm.densityDpi;// 屏幕密度（每寸像素：120, 160, 240, 320）
			String pathName = "";
			if (densityDPI == FolderCorrespondingScreenDensityEnum.DRAWABLE_HDPI.getScreenDensityValue()) {
				pathName = FolderCorrespondingScreenDensityEnum.DRAWABLE_HDPI.getFolderName();
			} else if (densityDPI == FolderCorrespondingScreenDensityEnum.DRAWABLE_LDPI.getScreenDensityValue()) {
				pathName = FolderCorrespondingScreenDensityEnum.DRAWABLE_LDPI.getFolderName();
			} else if (densityDPI == FolderCorrespondingScreenDensityEnum.DRAWABLE_MDPI.getScreenDensityValue()) {
				pathName = FolderCorrespondingScreenDensityEnum.DRAWABLE_MDPI.getFolderName();
			} else if (densityDPI == FolderCorrespondingScreenDensityEnum.DRAWABLE_XHDPI.getScreenDensityValue()) {
				pathName = FolderCorrespondingScreenDensityEnum.DRAWABLE_XHDPI.getFolderName();
			} else if (densityDPI == FolderCorrespondingScreenDensityEnum.DRAWABLE_XXHDPI.getScreenDensityValue()) {
				pathName = FolderCorrespondingScreenDensityEnum.DRAWABLE_XXHDPI.getFolderName();
			}
			return pathName = parentPathString + "/" + pathName;
		}

		/**
		 * 根据屏幕密度返回图片
		 * 
		 * @return
		 */
		@SuppressWarnings("deprecation")
    public static Drawable getBitmapAccordingScreenDensity(Context context, String bitmapName) {
			Bitmap bitmap = null;
			String filePathString = getPathAccordingScreenDensity(context) + "/" + bitmapName + ".png";
			bitmap = BitmapFactory.decodeFile(filePathString);
			Drawable drawable = null;
			do {
				if (bitmap == null) {
					break;
				}
				drawable = new BitmapDrawable(bitmap);
			} while (false);
			return drawable;

		}

		/**
		 * 根据价格字符串, 返回价格标签图片
		 * 
		 * @param context
		 * @param bitmapName
		 * @return
		 */
		@SuppressWarnings("deprecation")
    public static Drawable priceTagImageByPriceString(Context context, String bitmapName) {
			Bitmap bitmap = null;
			String priceString = ((int) Float.parseFloat(bitmapName)) + "";
			String filePathString = getPathAccordingScreenDensity(context) + "/price_" + priceString + ".png";
			bitmap = BitmapFactory.decodeFile(filePathString);
			Drawable drawable = null;
			do {
				if (bitmap == null) {
					break;
				}
				drawable = new BitmapDrawable(bitmap);
			} while (false);
			return drawable;
		};

		public static void replaceControlsBackgroung(String color, View view) {
			view.setBackgroundColor(Color.parseColor(color));
		}

		public static InputStream getInputStreamAccordingScreenDensity(Context context, String fileName) {
			File f = new File(getPathAccordingScreenDensity(context) + "/" + fileName + ".gif");

			InputStream in = null;
			try {
				in = new FileInputStream(f);
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			}
			return in;
		}

		public static byte[] InputStreamToByte(InputStream is) throws IOException {
			ByteArrayOutputStream bytestream = new ByteArrayOutputStream();
			int ch;
			while ((ch = is.read()) != -1) {
				bytestream.write(ch);
			}
			byte imgdata[] = bytestream.toByteArray();
			bytestream.close();
			return imgdata;
		}
	}

}
