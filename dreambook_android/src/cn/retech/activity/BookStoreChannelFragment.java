package cn.retech.activity;

import java.util.List;

import android.app.Activity;
import android.content.res.Configuration;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.RelativeLayout;
import cn.retech.adapter.BookGridViewAdapter;
import cn.retech.custom_control.MyGridView;
import cn.retech.domainbean_model.book.Book;
import cn.retech.domainbean_model.book.BookList;
import cn.retech.domainbean_model.booklist_in_bookstores.BookInfo;
import cn.retech.domainbean_model.booklist_in_bookstores.BookListInBookstoresNetRequestBean;
import cn.retech.domainbean_model.booklist_in_bookstores.BookListInBookstoresNetRespondBean;
import cn.retech.domainbean_model.login.LoginNetRespondBean;
import cn.retech.global_data_cache.GlobalDataCacheForMemorySingleton;
import cn.retech.my_network_engine.IDomainBeanAsyncHttpResponseListenerWithUIControl;
import cn.retech.my_network_engine.INetRequestHandle;
import cn.retech.my_network_engine.NetRequestHandleNilObject;
import cn.retech.my_network_engine.SimpleNetworkEngineSingleton;
import cn.retech.my_network_engine.net_error_handle.MyNetRequestErrorBean;
import cn.retech.toolutils.DebugLog;

public class BookStoreChannelFragment extends Fragment {
	private final String TAG = this.getClass().getSimpleName();

	public static interface OnHandlerListener {
		public void onBookClicked(Book book);
	}

	private OnHandlerListener onHandlerListener;

	public void setOnHandlerListener(OnHandlerListener onHandlerListener) {
		this.onHandlerListener = onHandlerListener;
	}

	private INetRequestHandle netRequestHandleForBookListInBookstores = new NetRequestHandleNilObject();
	private final BookGridViewAdapter bookStoreAdapter = new BookGridViewAdapter();
	private String identifier;
	private MyGridView gridView;
	// 书城图书列表(完成本地图书列表和从服务器新获取的图书列表进行了数据同步)
	private BookList bookList;

	// 网络加载等待提示框
	private RelativeLayout progressDialog;

	private LoginNetRespondBean bindAccount;

	public BookStoreChannelFragment() {
	}

	@Override
	public void onAttach(Activity activity) {
		super.onAttach(activity);
		Bundle bundle = getArguments();
		identifier = bundle.getString("identifier");
		bindAccount = (LoginNetRespondBean) bundle.getSerializable("bindAccount");
	}

	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);
	}

	@Override
	public void onPause() {
		super.onPause();

	}

	@Override
	public void onStart() {
		super.onStart();

	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);

		((MyGridView) getView().findViewById(R.id.book_list_gridView)).updateData();
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

	}

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		return inflater.inflate(R.layout.fragment_book_list, container, false);
	}

	@Override
	public void onViewCreated(View view, Bundle savedInstanceState) {

		progressDialog = (RelativeLayout) getView().findViewById(R.id.net_requestting_layout);

		gridView = (MyGridView) getView().findViewById(R.id.book_list_gridView);
		gridView.setAdapter(bookStoreAdapter);
		gridView.setOnItemClickListener(new AdapterView.OnItemClickListener() {

			@Override
			public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
				final List<Book> books = bookList.getShallowCopyBookList();
				final Book book = books.get(position);
				onHandlerListener.onBookClicked(book);
			}
		});

		if (bookList == null) {
			requestBookListByCategoryID(identifier);
		}
	}

	@Override
	public void onResume() {
		super.onResume();
	}

	@Override
	public void onStop() {
		netRequestHandleForBookListInBookstores.cancel();
		super.onStop();
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
	}

	@Override
	public void onDetach() {
		super.onDetach();
	}

	@Override
	public void onDestroyView() {
		super.onDestroyView();
	}

	/**
	 * 刷新
	 */
	public void refresh() {
		requestBookListByCategoryID(identifier);
	}

	private void requestBookListByCategoryID(final String categoryID) {

		BookListInBookstoresNetRequestBean netRequestBean = new BookListInBookstoresNetRequestBean();
		netRequestBean.setCategory_id(categoryID);
		netRequestHandleForBookListInBookstores = SimpleNetworkEngineSingleton.getInstance.requestDomainBean(netRequestBean, new IDomainBeanAsyncHttpResponseListenerWithUIControl() {

			@Override
			public void onFailure(MyNetRequestErrorBean error) {
				DebugLog.e(TAG, error.getErrorMessage());
			}

			@Override
			public void onSuccess(Object respondDomainBean) {
				if (bookList != null) {
					bookList.deleteObservers();
				}

				bookList = new BookList();
				BookListInBookstoresNetRespondBean bookListInBookstoresNetRespondBean = (BookListInBookstoresNetRespondBean) respondDomainBean;
				// 1，转型从网络获取的书籍列表
				// 2，将网络获取的书籍列表与本地书籍列表比对
				// 3，如果相同则从本地书籍列表中获取数据，加入到localbook中
				// 4，如果不同则将网络返回的书籍加入到localBook中
				// 5，刷新数据
				BookList localBookListFromLocal = GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList();// 本地书籍列表
				// java foreach 只会取一次迭代结合
				for (BookInfo bookInfo : bookListInBookstoresNetRespondBean.getBookInfoList()) {
					Book newBook = localBookListFromLocal.bookByContentID(bookInfo.getContent_id());
					if (newBook == null) {
						// 本地书籍列表不存在当前书籍
						newBook = new Book(bookInfo);
					} else {
						newBook.setInfo(bookInfo);
					}
					newBook.setBindAccount(bindAccount);
					bookList.addBook(newBook);
				}

				bookStoreAdapter.changeDataSource(bookList.getShallowCopyBookList());

			}

			@Override
			public void onBegin() {
				progressDialog.setVisibility(View.VISIBLE);

			}

			@Override
			public void onEnd() {
				progressDialog.setVisibility(View.INVISIBLE);
			}
		});

	}
}
