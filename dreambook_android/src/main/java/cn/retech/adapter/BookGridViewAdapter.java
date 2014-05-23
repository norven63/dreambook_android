package cn.retech.adapter;

import java.util.ArrayList;
import java.util.List;

import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import cn.retech.activity.MyApplication;
import cn.retech.custom_control.BookStoreBookCell;
import cn.retech.domainbean_model.book.Book;

public class BookGridViewAdapter extends BaseAdapter {
	private List<Book> dataSource = new ArrayList<Book>();

	public void changeDataSource(final List<Book> newDataSource) {
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
		Book book = (Book) getItem(position);
		BookStoreBookCell bookCell = new BookStoreBookCell(MyApplication.getApplication());
		bookCell.bind(book);

		return bookCell;
	}

}
