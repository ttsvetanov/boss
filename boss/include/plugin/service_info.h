#ifndef __BOSS_PLUGIN_SERVICE_INFO_H__
#define __BOSS_PLUGIN_SERVICE_INFO_H__

#include "../core/base.h"
#include "../core/error_codes.h"
#include "../core/ref_obj_ptr.h"
#include "../common/enum.h"
#include "../common/entity_id.h"
#include "../common/string.h"
#include "iservice_info.h"

#include <string>

namespace Boss
{
  
  namespace Private
  {

    template <typename T, bool = !!std::is_base_of<IServiceInfo, T>::value>
    class ServiceInfo;
    
    template <typename T>
    class ServiceInfo<T, true>
      : public CoClass<Crc32("Boss.ServiceInfo"), T>
    {
    public:
      ServiceInfo()
        : SrvId(ServiceId())
      {
      }
      void SetServiceId(ServiceId srvId)
      {
        SrvId = srvId;
      }
      void AddCoClassId(ClassId clsId)
      {
        if (!CoClassIds.Get())
          CoClassIds = Base<Enum>::Create();
        CoClassIds->AddItem(Base<EntityId>::Create(clsId));
      }
      void AddCoClassIds(RefObjPtr<IEnum> coClassIds)
      {
        if (!coClassIds.Get())
          return;
        if (!CoClassIds.Get())
          CoClassIds = Base<Enum>::Create();
        coClassIds->Reset();
        for (RefObjPtr<IBase> i ; coClassIds->Next(i.GetPPtr()) == Status::Ok ; i.Release())
          CoClassIds->AddItem(i);
      }
      
    private:
      ServiceId SrvId;
      mutable RefObjPtr<Enum> CoClassIds;
      
      // IServiceInfo
      virtual RetCode BOSS_CALL GetServiceId(ServiceId *serviceId) const
      {
        if (!serviceId)
          return Status::InvalidArgument;
        *serviceId = SrvId;
        return Status::Ok;
      }
      virtual RetCode BOSS_CALL GetClassIds(IEnum **ids) const
      {
        return !ids || *ids ? Status::InvalidArgument : CoClassIds.QueryInterface(ids);
      }
    };
    
  }

  class LocalServiceInfo
    : public CoClass<Crc32("Boss.LocalServiceInfo"), Private::ServiceInfo<ILocalServiceInfo>>
  {
  public:
    void SetModulePath(std::string const &path)
    {
      Path = Base<String>::Create(path);
    }
    void SetModulePath(RefObjPtr<IString> path)
    {
      Path = path;
    }
    
  private:
    mutable RefObjPtr<IString> Path;
    
    // ILocalServiceInfo
    virtual RetCode BOSS_CALL GetModulePath(IString **path) const
    {
      return !path || *path ? Status::NotFound : Path.QueryInterface(path);
    }
  };
  
  class RemoteServiceInfo
    : public CoClass<Crc32("Boss.RemoteServiceInfo"), Private::ServiceInfo<IRemoteServiceInfo>>
  {
  public:
    void SetProps(RefObjPtr<IPropertyBag> props)
    {
      Props = props;
    }
    
  private:
    mutable RefObjPtr<IPropertyBag> Props;
    
    // IRemoteServiceInfo
    virtual RetCode BOSS_CALL GetProperties(IPropertyBag **props) const
    {
      return !props || *props ? Status::NotFound : Props.QueryInterface(props);
    }
  };

}

#endif  // !__BOSS_PLUGIN_SERVICE_INFO_H__
