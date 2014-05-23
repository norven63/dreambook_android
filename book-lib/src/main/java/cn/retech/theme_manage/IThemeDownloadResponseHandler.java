package cn.retech.theme_manage;

import cn.retech.my_network_engine.net_error_handle.MyNetRequestErrorBean;

public interface IThemeDownloadResponseHandler {
	/**
	 * 任务完成
	 * 
	 */
	public void onMissionComplete();

	/**
	 * 请求主题失败,这里不应该用NetErrorBean
	 * 
	 * @param error
	 */
	public void onFailure(final MyNetRequestErrorBean error);

	/**
	 * 主题下载进度
	 * 
	 * @param bytesWritten
	 *            已完成的数据长度
	 * @param totalSize
	 *            要下载的数据总长度
	 */
	public void onProgressDoInBackground(final long bytesWritten, final long totalSize);

	/**
	 * 主题下载完成
	 * 
	 */
	public void onDownloadSuccess();
}
