package cn.retech.rich_text_reader;

public class BookReaderJni {
	static {
		System.loadLibrary("testiconv");
	}

	public static native void initGlobalSettings(int width, int height, int densityDPI);

	public static native void setListeners(Object crbClickListener, Object calcLayoutObserver);

	public static native void drawBookContent(int[] rectValues, Object bitmap, boolean isFullScreen);

	public static native boolean loadBook(String bookPath);

	public static native boolean nextPage();

	public static native boolean lastPage();

	public static native boolean goToPage(int pageNumber);

	public static native boolean setFontSize(int fontSize);

	public static native boolean isCurPosVisible();

	public static native void dragComment(float x, float y);

	public static native void selSentence(float x, float y);

	public static native boolean dragSentence(float x, float y);

	public static native void onClick(float x, float y);

	public static native void onTouchMove(float x, float y, int[] rectValues);

	public static native void removeBookmark();
}
