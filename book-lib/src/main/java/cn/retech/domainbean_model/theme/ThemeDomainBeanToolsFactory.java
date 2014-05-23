package cn.retech.domainbean_model.theme;

import cn.retech.global_data_cache.UrlConstantForThisProject;
import cn.retech.my_network_engine.domainbean_tools.IDomainBeanAbstractFactory;
import cn.retech.my_network_engine.domainbean_tools.IParseDomainBeanToDataDictionary;
import cn.retech.my_network_engine.domainbean_tools.IParseNetRespondDataToDomainBean;

/**
 * app 主题
 * 
 */
public final class ThemeDomainBeanToolsFactory implements IDomainBeanAbstractFactory {
	@Override
	public IParseDomainBeanToDataDictionary getParseDomainBeanToDDStrategyObject() {
		return new ThemeParseDomainBeanToDD();
	}

	@Override
	public IParseNetRespondDataToDomainBean getParseNetRespondDataToDomainBeanStrategyObject() {
		return new ThemeParseNetRespondStringToDomainBean();
	}

	@Override
	public String getSpecialPath(final Object netRequestDomainBean) {
		return UrlConstantForThisProject.kUrlConstant_SpecialPath_theme;
	}
}
