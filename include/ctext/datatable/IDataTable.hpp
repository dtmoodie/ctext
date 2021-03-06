#ifndef CT_EXT_IDATA_TABLE_HPP
#define CT_EXT_IDATA_TABLE_HPP
#include "DataTableArrayIterator.hpp"
#include "DataTableStorage.hpp"

#include <ct/reflect.hpp>
#include <ct/reflect_traits.hpp>

namespace ct {
namespace ext {

template <class T> struct ElementView {
  T *begin() const { return m_begin; }
  T *end() const { return m_end; }

  T *m_begin;
  T *m_end;
};

template <class DTYPE, class BASES = typename Reflect<DTYPE>::BaseTypes>
struct IDataTable;

template <class T> struct TensorOf {
  using DType = typename DataDimensionality<T>::DType;
  static constexpr const uint8_t Dims = DataDimensionality<T>::value;
  using Type = mt::Tensor<DType, Dims + 1>;
  using ConstType = mt::Tensor<const DType, Dims + 1>;
};

template <class T> using TensorOf_t = typename TensorOf<T>::Type;

template <class T> using ConstTensorOf_t = typename TensorOf<T>::ConstType;

template <class DTYPE> struct IDataTable<DTYPE, VariadicTypedef<>> {
  virtual ~IDataTable() = default;

  template <class T, class U>
  auto begin(T U::*mem_ptr)
      -> EnableIf<DataDimensionality<T>::value == 0, T *> {
    static_assert(std::is_same<U, DTYPE>::value ||
                      IsBase<Base<DTYPE>, Derived<U>>::value ||
                      IsBase<Base<U>, Derived<DTYPE>>::value,
                  "Mem ptr must derive from DTYPE");
    const size_t offset = memberOffset(mem_ptr);
    auto p = ptr(offset, 0);
    return ptrCast<T>(p.data());
  }

  /*template <class T, class U>
  DataTableArrayIterator<T> begin(TArrayView<T> U::*mem_ptr)
  {
      static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>,
  Derived<U>>::value ||
                        IsBase<Base<U>, Derived<DTYPE>>::value,
                    "Mem ptr must derive from DTYPE");
      const size_t offset = memberOffset(mem_ptr);
      auto p = ptr(offset, 0);
      return p;
  }*/

  template <class T, class U>
  auto begin(T U::*mem_ptr) const
      -> ct::EnableIf<DataDimensionality<T>::value == 0, const T *> {
    static_assert(std::is_same<U, DTYPE>::value ||
                      IsBase<Base<DTYPE>, Derived<U>>::value ||
                      IsBase<Base<U>, Derived<DTYPE>>::value,
                  "Mem ptr must derive from DTYPE");
    const size_t offset = memberOffset(mem_ptr);
    auto p = ptr(offset, 0);
    const auto ptr = p.data();
    return ptrCast<T>(ptr);
  }

  template <class T, class U>
  auto begin(T U::*mem_ptr) const
      -> ct::EnableIf<DataDimensionality<T>::value != 0, ConstTensorOf_t<T>> {
    static_assert(std::is_same<U, DTYPE>::value ||
                      IsBase<Base<DTYPE>, Derived<U>>::value ||
                      IsBase<Base<U>, Derived<DTYPE>>::value,
                  "Mem ptr must derive from DTYPE");
    const size_t offset = memberOffset(mem_ptr);
    auto p = ptr(offset, 0);
    return p;
  }

  template <class T, class U>
  auto end(T U::*mem_ptr)
      -> ct::EnableIf<DataDimensionality<T>::value == 0, T *> {
    static_assert(std::is_same<U, DTYPE>::value ||
                      IsBase<Base<DTYPE>, Derived<U>>::value,
                  "Mem ptr must derive from DTYPE");
    const size_t offset = memberOffset(mem_ptr);
    const size_t sz = size();
    auto p = ptr(offset, sz);
    const auto ptr = p.data();
    return ptrCast<T>(ptr);
  }

  template <class T, class U>
  auto end(T U::*mem_ptr)
      -> EnableIf<DataDimensionality<T>::value != 0, TensorOf_t<T>> {
    static_assert(std::is_same<U, DTYPE>::value ||
                      IsBase<Base<DTYPE>, Derived<U>>::value,
                  "Mem ptr must derive from DTYPE");
    const size_t offset = memberOffset(mem_ptr);
    const size_t sz = size();
    auto p = ptr(offset, sz);
    return p;
  }

  template <class T, class U>
  auto end(T U::*mem_ptr) const
      -> EnableIf<DataDimensionality<T>::value == 0, T *> {
    static_assert(std::is_same<U, DTYPE>::value ||
                      IsBase<Base<DTYPE>, Derived<U>>::value,
                  "Mem ptr must derive from DTYPE");
    const size_t offset = memberOffset(mem_ptr);
    const size_t sz = size();
    auto p = ptr(offset, sz);
    const auto ptr = p.data();
    return ptrCast<T>(ptr);
  }

  template <class T, class U>
  auto end(T U::*mem_ptr) const
      -> EnableIf<DataDimensionality<T>::value != 0, ConstTensorOf_t<T>> {
    static_assert(std::is_same<U, DTYPE>::value ||
                      IsBase<Base<DTYPE>, Derived<U>>::value,
                  "Mem ptr must derive from DTYPE");
    const size_t offset = memberOffset(mem_ptr);
    const size_t sz = size();
    auto p = ptr(offset, sz);
    return p;
  }

  template <class T, class U> ElementView<T> view(T U::*mem_ptr) {
    auto begin_ = begin(mem_ptr);
    auto end_ = end(mem_ptr);
    return {begin_, end_};
  }

  template <class T, class U> ElementView<const T> view(T U::*mem_ptr) const {
    auto begin_ = begin(mem_ptr);
    auto end_ = end(mem_ptr);
    return {begin_, end_};
  }

  virtual size_t size() const = 0;

  /**
   * @brief populateData populates a struct DTYPE with the data from element idx
   * in the table
   * @param out
   * @param index of the
   */
  // virtual void populateData(DTYPE& out, size_t index) {
  // populateDataRecurse(out, index,
  // Reflect<DTYPE>::end()); }
  virtual void populateData(DTYPE &out, size_t index) = 0;

private:
  virtual mt::Tensor<void, 2> ptr(size_t offset, size_t idx) = 0;
  virtual mt::Tensor<const void, 2> ptr(size_t offset, size_t idx) const = 0;
};

template <class DTYPE, class BASE_TYPE>
struct IDataTable<DTYPE, VariadicTypedef<BASE_TYPE>>
    : virtual IDataTable<BASE_TYPE> {
  using IDataTable<BASE_TYPE>::populateData;

  virtual ~IDataTable() = default;
  template <class T, class U> T *begin(T U::*mem_ptr) {
    static_assert(std::is_same<U, DTYPE>::value ||
                      IsBase<Base<DTYPE>, Derived<U>>::value ||
                      IsBase<Base<U>, Derived<DTYPE>>::value,
                  "Mem ptr must derive from DTYPE");
    const size_t offset = memberOffset(mem_ptr);
    auto p = ptr(offset, 0);
    return p.template ptr<T>();
  }

  template <class T, class U>
  mt::Tensor<T, 2> begin(TArrayView<T> U::*mem_ptr) {
    static_assert(std::is_same<U, DTYPE>::value ||
                      IsBase<Base<DTYPE>, Derived<U>>::value ||
                      IsBase<Base<U>, Derived<DTYPE>>::value,
                  "Mem ptr must derive from DTYPE");
    const size_t offset = memberOffset(mem_ptr);
    auto p = ptr(offset, 0);
    return p;
  }

  template <class T, class U> const T *begin(T U::*mem_ptr) const {
    static_assert(std::is_same<U, DTYPE>::value ||
                      IsBase<Base<DTYPE>, Derived<U>>::value ||
                      IsBase<Base<U>, Derived<DTYPE>>::value,
                  "Mem ptr must derive from DTYPE");
    const size_t offset = memberOffset(mem_ptr);
    auto p = ptr(offset, 0);
    return ptrCast<T>(p.data());
  }

  template <class T, class U>
  mt::Tensor<const T, 2> begin(TArrayView<T> U::*mem_ptr) const {
    static_assert(std::is_same<U, DTYPE>::value ||
                      IsBase<Base<DTYPE>, Derived<U>>::value ||
                      IsBase<Base<U>, Derived<DTYPE>>::value,
                  "Mem ptr must derive from DTYPE");
    const size_t offset = memberOffset(mem_ptr);
    auto p = ptr(offset, 0);
    return p;
  }

  template <class T, class U> T *end(T U::*mem_ptr) {
    static_assert(std::is_same<U, DTYPE>::value ||
                      IsBase<Base<DTYPE>, Derived<U>>::value,
                  "Mem ptr must derive from DTYPE");
    const size_t offset = memberOffset(mem_ptr);
    const size_t sz = size();
    auto p = ptr(offset, sz);
    return ptrCast<T>(p.data());
  }

  template <class T, class U> mt::Tensor<T, 2> end(TArrayView<T> U::*mem_ptr) {
    static_assert(std::is_same<U, DTYPE>::value ||
                      IsBase<Base<DTYPE>, Derived<U>>::value,
                  "Mem ptr must derive from DTYPE");
    const size_t offset = memberOffset(mem_ptr);
    const size_t sz = size();
    auto p = ptr(offset, sz);
    return p;
  }

  template <class T, class U> const T *end(T U::*mem_ptr) const {
    static_assert(std::is_same<U, DTYPE>::value ||
                      IsBase<Base<DTYPE>, Derived<U>>::value,
                  "Mem ptr must derive from DTYPE");
    const size_t offset = memberOffset(mem_ptr);
    const size_t sz = size();
    auto p = ptr(offset, sz);
    return ptrCast<T>(p.data());
  }

  template <class T, class U>
  mt::Tensor<const T, 2> end(TArrayView<T> U::*mem_ptr) const {
    static_assert(std::is_same<U, DTYPE>::value ||
                      IsBase<Base<DTYPE>, Derived<U>>::value,
                  "Mem ptr must derive from DTYPE");
    const size_t offset = memberOffset(mem_ptr);
    const size_t sz = size();
    auto p = ptr(offset, sz);
    return p;
  }

  template <class T, class U> ElementView<T> view(T U::*mem_ptr) {
    return {begin(mem_ptr), end(mem_ptr)};
  }

  template <class T, class U> ElementView<const T> view(T U::*mem_ptr) const {
    return {begin(mem_ptr), end(mem_ptr)};
  }

  virtual size_t size() const = 0;

  /**
   * @brief populateData populates a struct DTYPE with the data from element idx
   * in the table
   * @param out
   * @param index of the
   */
  // virtual void populateData(DTYPE& out, size_t index) {
  // populateDataRecurse(out, index,
  // Reflect<DTYPE>::end()); }
  virtual void populateData(DTYPE &out, size_t index) = 0;

private:
  virtual mt::Tensor<void, 2> ptr(size_t offset, size_t idx) = 0;
  virtual mt::Tensor<const void, 2> ptr(size_t offset, size_t idx) const = 0;
};

struct IComponentProvider {
  virtual ~IComponentProvider() = default;
  virtual bool providesComponent(const std::type_info &) const = 0;

  virtual uint32_t getNumComponents() const = 0;
  virtual const std::type_info *getComponentType(uint32_t idx) const = 0;

  virtual IComponentProvider *getProvider(const std::type_info &) = 0;
  virtual const IComponentProvider *
  getProvider(const std::type_info &) const = 0;

  template <class T> bool getComponentMutable(TArrayView<T> &);
  template <class T> bool getComponent(TArrayView<const T> &);

  virtual size_t getNumEntities() const = 0;
};

template <class T> struct TComponentProvider : IComponentProvider {
  bool providesComponent(const std::type_info &info) const override {
    return &info == &typeid(T);
  }
  // We only get views to the data since we are mutating the fields of an
  // entity, we do NOT want to add
  // new entities with this interface since we would be breaking the associating
  // with other components.
  // At the table level we can add or remove an entity, but not at the component
  // level
  virtual void getComponentMutable(TArrayView<T> &) = 0;
  virtual void getComponent(TArrayView<const T> &) const = 0;
};

template <class T>
struct TComponentProvider<TArrayView<T>> : IComponentProvider {
  bool providesComponent(const std::type_info &info) const override {
    return &info == &typeid(T);
  }
  // We only get views to the data since we are mutating the fields of an
  // entity, we do NOT want to add
  // new entities with this interface since we would be breaking the associating
  // with other components.
  // At the table level we can add or remove an entity, but not at the component
  // level
  virtual void getComponentMutable(DataTableArrayIterator<T> &) = 0;
  virtual void getComponent(DataTableArrayIterator<const T> &) const = 0;
};

template <class DERIVED, class T>
struct TComponentProviderImpl : IComponentProvider {
  bool providesComponent(const std::type_info &) const override {
    return false;
  }
  IComponentProvider *getProvider(const std::type_info &) override {
    return nullptr;
  }
  const IComponentProvider *getProvider(const std::type_info &) const override {
    return nullptr;
  }
  virtual uint32_t getNumComponents() const { return 0; };
  virtual const std::type_info *getComponentType(uint32_t) const override {
    return nullptr;
  }
  size_t getNumEntities() const override { return 0; };
};

template <class DERIVED, class T>
struct TComponentProviderImpl<DERIVED, VariadicTypedef<T>>
    : TComponentProvider<T> {

  IComponentProvider *getProvider(const std::type_info &type) override {
    if (&type == &typeid(T)) {
      return this;
    }
    return nullptr;
  }

  const IComponentProvider *
  getProvider(const std::type_info &type) const override {
    if (&type == &typeid(T)) {
      return this;
    }
    return nullptr;
  }

  void getComponentMutable(TArrayView<T> &out) {
    using DType = typename DERIVED::DType;
    constexpr const index_t I = indexOfMemberType<DType, T>();
    static_assert(I != -1, "");
    constexpr const auto mem_ptr = ct::Reflect<DType>::getPtr(Indexer<I>{});
    const auto size = static_cast<DERIVED *>(this)->size();
    auto ptr = static_cast<DERIVED *>(this)->begin(mem_ptr.m_ptr);
    out = TArrayView<T>(ptr, size);
  }

  void getComponent(TArrayView<const T> &out) const {
    using DType = typename DERIVED::DType;
    constexpr const index_t I = indexOfMemberType<DType, T>();
    static_assert(I != -1, "");
    constexpr const auto mem_ptr = ct::Reflect<DType>::getPtr(Indexer<I>{});
    const auto ptr = static_cast<const DERIVED *>(this)->begin(mem_ptr.m_ptr);
    const auto size = static_cast<const DERIVED *>(this)->size();
    out = TArrayView<const T>(ptr, size);
  }

  uint32_t getNumComponents() const override { return 1; }
  const std::type_info *getComponentType(uint32_t idx) const override {
    if (idx == 0) {
      return &typeid(T);
    }
    return nullptr;
  }
  size_t getNumEntities() const override {
    return static_cast<const DERIVED *>(this)->size();
  };
};

template <class DERIVED, class T, class... U>
struct TComponentProviderImpl<DERIVED, VariadicTypedef<T, U...>>
    : TComponentProvider<T>,
      TComponentProviderImpl<DERIVED, VariadicTypedef<U...>> {
  using super = TComponentProviderImpl<DERIVED, VariadicTypedef<U...>>;

  IComponentProvider *getProvider(const std::type_info &type) override {
    IComponentProvider *out = super::getProvider(type);
    if (!out && &type == &typeid(T)) {
      out = static_cast<TComponentProvider<T> *>(this);
    }
    return out;
  }

  const IComponentProvider *
  getProvider(const std::type_info &type) const override {
    const IComponentProvider *out = super::getProvider(type);
    if (!out && &type == &typeid(T)) {
      out = static_cast<const TComponentProvider<T> *>(this);
    }
    return out;
  }

  bool providesComponent(const std::type_info &type) const override {
    if (super::providesComponent(type)) {
      return true;
    }
    return TComponentProvider<T>::providesComponent(type);
  }

  void getComponentMutable(TArrayView<T> &out) {
    using DType = typename DERIVED::DType;
    constexpr const index_t I = indexOfMemberType<DType, T>();
    static_assert(I != -1, "");
    constexpr const auto mem_ptr = ct::Reflect<DType>::getPtr(Indexer<I>{});
    const auto size = static_cast<DERIVED *>(this)->size();
    auto ptr = static_cast<DERIVED *>(this)->begin(mem_ptr.m_ptr);
    out = TArrayView<T>(ptr, size);
  }

  void getComponent(TArrayView<const T> &out) const {
    using DType = typename DERIVED::DType;
    constexpr const index_t I = indexOfMemberType<DType, T>();
    static_assert(I != -1, "");
    constexpr const auto mem_ptr = ct::Reflect<DType>::getPtr(Indexer<I>{});
    const auto ptr = static_cast<const DERIVED *>(this)->begin(mem_ptr.m_ptr);
    const auto size = static_cast<const DERIVED *>(this)->size();
    out = TArrayView<const T>(ptr, size);
  }

  uint32_t getNumComponents() const override { return sizeof...(U) + 1; }
  const std::type_info *getComponentType(uint32_t idx) const override {
    if (idx == 0) {
      return &typeid(T);
    }
    return super::getComponentType(idx - 1);
  }
  size_t getNumEntities() const override {
    return static_cast<const DERIVED *>(this)->size();
  };
};

template <class T>
bool IComponentProvider::getComponentMutable(TArrayView<T> &out) {
  auto provider = getProvider(typeid(T));
  if (!provider) {
    return false;
  }
  auto typed = static_cast<TComponentProvider<T> *>(provider);
  typed->getComponentMutable(out);
  return true;
}

template <class T>
bool IComponentProvider::getComponent(TArrayView<const T> &out) {
  auto provider = getProvider(typeid(T));
  if (!provider) {
    return false;
  }
  provider->getComponent(out);
  return true;
}

template <class DTYPE, class DERIVED,
          class DERIVED_TYPES = typename Reflect<DTYPE>::BaseTypes>
struct IDataTableImpl;

template <class DTYPE, class DERIVED, class BASE_TYPE>
struct IDataTableImpl<DTYPE, DERIVED, VariadicTypedef<BASE_TYPE>>
    : virtual IDataTable<DTYPE>, IDataTableImpl<BASE_TYPE, DERIVED> {
  using IDataTableImpl<BASE_TYPE, DERIVED>::populateData;
  void populateData(DTYPE &out, size_t index) override {
    static_cast<DERIVED *>(this)->populateDataRecurse(out, index,
                                                      Reflect<DTYPE>::end());
  }
};

template <class DTYPE, class DERIVED>
struct IDataTableImpl<DTYPE, DERIVED, VariadicTypedef<>>
    : virtual IDataTable<DTYPE> {
  void populateData(DTYPE &out, size_t index) override {
    static_cast<DERIVED *>(this)->populateDataRecurse(out, index,
                                                      Reflect<DTYPE>::end());
  }
};
} // namespace ext
} // namespace ct
#endif // CT_EXT_IDATA_TABLE_HPP
