package cn.retech.adapter;

import java.util.ArrayList;
import java.util.List;

import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import cn.retech.activity.MyApplication;
import cn.retech.custom_control.BookFolderLayout;
import cn.retech.custom_control.BookShelfBookCell;
import cn.retech.domainbean_model.book.BookFile;

public class Drag_BookGridViewAdapter extends BaseAdapter {
	private List<BookFile> dataSource = new ArrayList<BookFile>();

	public void changeDataSource(final List<BookFile> newDataSource) {
		if (newDataSource == null) {
			assert false : "入参 newDataSource 为空. ";
			return;
		}

		this.dataSource = newDataSource;
		this.notifyDataSetChanged();
	}

	@Override
	public int getCount() {
		return dataSource.size();
	}

	@Override
	public Object getItem(int position) {
		return dataSource.get(position);
	}

	@Override
	public long getItemId(int position) {
		return position;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		BookFile bookFile = (BookFile) getItem(position);
		View bookCell = null;
		/**
		 * 在组装书架界面时,判断每个Cell的类型: 若为文件,则直接创建对应的BookShelfBookCell;
		 * 若为文件夹,则先创建对应的BookFolderLayout ,然后迭代其子元素,
		 * 为每个子元素创建对应的BookShelfBookCell,最后依次添加至BookFolderLayout视图中
		 */
		if (bookFile.isFile()) {
			// 直接创建BookShelfBookCell
			bookCell = new BookShelfBookCell(MyApplication.getApplication());
			((BookShelfBookCell) bookCell).bind(bookFile);
		} else {
			BookFolderLayout bookFolderLayout = new BookFolderLayout(MyApplication.getApplication());
			bookFolderLayout.setBookFile(bookFile);// 设置UI和数据层之间的关联关系

			// 迭代并添加之
			List<BookFile> bookFiles = bookFile.getListFiles();
			for (BookFile bookFileItem : bookFiles) {
				BookShelfBookCell bookShelfCell = new BookShelfBookCell(MyApplication.getApplication());
				bookShelfCell.bind(bookFileItem);

				bookFolderLayout.addChildView(bookShelfCell);
			}

			bookCell = bookFolderLayout;
		}

		return bookCell;
	}

}
