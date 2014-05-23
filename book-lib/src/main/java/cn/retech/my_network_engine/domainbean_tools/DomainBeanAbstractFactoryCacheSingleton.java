package cn.retech.my_network_engine.domainbean_tools;

import java.util.Map;

import android.text.TextUtils;
import cn.retech.domainbean_model.DomainBeanHelperClassNameMapping;

import com.google.common.collect.Maps;

/**
 * NetRequestBean对应的DomainBeanToolsFactory, 在这里进行缓存
 * 
 * @author skyduck
 */
public enum DomainBeanAbstractFactoryCacheSingleton {
  getInstance;

  /**
	 * 
	 */
  private DomainBeanHelperClassNameMapping strategyClassNameMapping = new DomainBeanHelperClassNameMapping();
  /**
   * 缓存区
   */
  private Map<String, IDomainBeanAbstractFactory> abstractFactoryObjectBufList = Maps.newHashMapWithExpectedSize(100);

  /**
   * 
   * @param abstractFactoryMappingKey
   *          就是某个网络业务接口的 NetRequestBean的 getClass().getName() , 这里设计一个字典,
   *          用于缓存一个 NetRequestBean和其对应的 DomainBeanToolsFactory
   * @return
   * @throws Exception
   */
  public IDomainBeanAbstractFactory getDomainBeanAbstractFactoryObjectByKey(final String abstractFactoryMappingKey) throws Exception {

    if (TextUtils.isEmpty(abstractFactoryMappingKey)) {
      throw new NullPointerException("传入的请求业务Bean, 关键字符串(abstractFactoryMappingKey)为空!");
    }

    IDomainBeanAbstractFactory abstractFactoryObject = abstractFactoryObjectBufList.get(abstractFactoryMappingKey);
    if (abstractFactoryObject == null) {
      final String className = strategyClassNameMapping.getTargetClassNameForKey(abstractFactoryMappingKey);
      if (TextUtils.isEmpty(className)) {
        // 找不到对应的算法类
        throw new IllegalStateException("传入的请求业务Bean, 关键字符串(abstractFactoryMappingKey=" + abstractFactoryMappingKey + ")找不到与其对应的 抽象工厂类!");
      }

      // 使用 "反射" 来创建一个NetRequestBean对应的DomainBeanToolsFactory
      final Class<?> cl = Class.forName(className);
      abstractFactoryObject = (IDomainBeanAbstractFactory) cl.newInstance();
      abstractFactoryObjectBufList.put(abstractFactoryMappingKey, abstractFactoryObject);
    }
    return abstractFactoryObject;
  }
}
