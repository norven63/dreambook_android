package cn.retech.domainbean_model.get_book_download_url;

import cn.retech.global_data_cache.UrlConstantForThisProject;
import cn.retech.my_network_engine.domainbean_tools.IDomainBeanAbstractFactory;
import cn.retech.my_network_engine.domainbean_tools.IParseDomainBeanToDataDictionary;
import cn.retech.my_network_engine.domainbean_tools.IParseNetRespondDataToDomainBean;

/**
 * 获取书籍下载URL
 * 
 */
public final class GetBookDownloadUrlDomainBeanToolsFactory implements IDomainBeanAbstractFactory {
	@Override
	public IParseDomainBeanToDataDictionary getParseDomainBeanToDDStrategyObject() {
		return new GetBookDownloadUrlParseDomainBeanToDD();
	}

	@Override
	public IParseNetRespondDataToDomainBean getParseNetRespondDataToDomainBeanStrategyObject() {
		return new GetBookDownloadUrlParseNetRespondStringToDomainBean();
	}

	@Override
	public String getSpecialPath(final Object netRequestDomainBean) {
	  GetBookDownloadUrlNetRequestBean getBookDownloadUrlNetRequestBean = (GetBookDownloadUrlNetRequestBean)netRequestDomainBean;
		return UrlConstantForThisProject.kUrlConstant_SpecialPath_book_downlaod_url + getBookDownloadUrlNetRequestBean.getContentId();
	}
}
