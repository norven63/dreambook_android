package cn.retech.my_network_engine.engine_helper;

import cn.retech.my_custom_network_engine.engine_helper.NetRequestEntityDataPackageForDreamBook;
import cn.retech.my_custom_network_engine.engine_helper.NetRespondEntityDataUnpackForDreamBook;
import cn.retech.my_custom_network_engine.engine_helper.ServerRespondDataTestForDreamBook;
import cn.retech.my_custom_network_engine.engine_helper.SpliceFullUrlByDomainBeanSpecialPathForDreamBook;
import cn.retech.my_network_engine.engine_helper.interfaces.INetRequestEntityDataPackage;
import cn.retech.my_network_engine.engine_helper.interfaces.INetRespondRawEntityDataUnpack;
import cn.retech.my_network_engine.engine_helper.interfaces.IServerRespondDataTest;
import cn.retech.my_network_engine.engine_helper.interfaces.ISpliceFullUrlByDomainBeanSpecialPath;

/**
 * 引擎助手函数
 * 
 * @author skyduck
 */
public enum EngineHelperSingleton implements IEngineHelper {
  getInstance;

  @Override
  public INetRequestEntityDataPackage getNetRequestEntityDataPackageStrategyObject() {
    return new NetRequestEntityDataPackageForDreamBook();
  }

  @Override
  public INetRespondRawEntityDataUnpack getNetRespondEntityDataUnpackStrategyObject() {
    return new NetRespondEntityDataUnpackForDreamBook();
  }

  @Override
  public IServerRespondDataTest getServerRespondDataTestStrategyObject() {
    return new ServerRespondDataTestForDreamBook();
  }

  @Override
  public ISpliceFullUrlByDomainBeanSpecialPath getSpliceFullUrlByDomainBeanSpecialPathStrategyObject() {
    return new SpliceFullUrlByDomainBeanSpecialPathForDreamBook();
  }

}
