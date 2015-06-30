package cn.retech.custom_control;

import cn.retech.domainbean_model.book.BookFile;

public interface DragView {
	public void hiddenSelf();

	public void showSelf();

	public BookFile getBookFile();
}
