package cn.retech.domainbean_model.book_categories;

import cn.retech.global_data_cache.UrlConstantForThisProject;
import cn.retech.my_network_engine.domainbean_tools.IDomainBeanAbstractFactory;
import cn.retech.my_network_engine.domainbean_tools.IParseDomainBeanToDataDictionary;
import cn.retech.my_network_engine.domainbean_tools.IParseNetRespondDataToDomainBean;

/**
 * 获取图书分类
 * 
 */
public final class BookCategoriesDomainBeanToolsFactory implements IDomainBeanAbstractFactory {
	@Override
	public IParseDomainBeanToDataDictionary getParseDomainBeanToDDStrategyObject() {
		return null;
	}

	@Override
	public IParseNetRespondDataToDomainBean getParseNetRespondDataToDomainBeanStrategyObject() {
		return new BookCategoriesParseNetRespondStringToDomainBean();
	}

	@Override
	public String getSpecialPath(final Object netRequestDomainBean) {
		return UrlConstantForThisProject.kUrlConstant_SpecialPath_book_categories;
	}
}
