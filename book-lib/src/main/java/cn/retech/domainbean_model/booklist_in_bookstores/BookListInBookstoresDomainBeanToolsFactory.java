package cn.retech.domainbean_model.booklist_in_bookstores;

import cn.retech.global_data_cache.UrlConstantForThisProject;
import cn.retech.my_network_engine.domainbean_tools.IDomainBeanAbstractFactory;
import cn.retech.my_network_engine.domainbean_tools.IParseDomainBeanToDataDictionary;
import cn.retech.my_network_engine.domainbean_tools.IParseNetRespondDataToDomainBean;

/**
 * 获取图书列表
 * 
 */
public final class BookListInBookstoresDomainBeanToolsFactory implements IDomainBeanAbstractFactory {
	@Override
	public IParseDomainBeanToDataDictionary getParseDomainBeanToDDStrategyObject() {
		return new BookListInBookstoresParseDomainBeanToDD();
	}

	@Override
	public IParseNetRespondDataToDomainBean getParseNetRespondDataToDomainBeanStrategyObject() {
		return new BookListInBookstoresParseNetRespondStringToDomainBean();
	}

	@Override
	public String getSpecialPath(final Object netRequestDomainBean) {
		return UrlConstantForThisProject.kUrlConstant_SpecialPath_book_list_in_bookstores;
	}
}
