/*
 * Copyright 2014 Google Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FRUIT_COMPONENT_DEFN_H
#define FRUIT_COMPONENT_DEFN_H

#include "../component.h"

#include "injection_errors.h"
#include "storage/component_storage.h"

#include <memory>

namespace fruit {

template <typename... Params>
template <typename OtherComp>
inline Component<Params...>::Component(PartialComponent<OtherComp> component)
  : PartialComponent<fruit::impl::meta::Apply<fruit::impl::meta::ConstructComponentImpl, Params...>>(std::move(component)) {
  using DestComp = fruit::impl::meta::Apply<fruit::impl::meta::ConstructComponentImpl, Params...>;
  (void)typename fruit::impl::meta::CheckIfError<DestComp>::type();
  // Keep in sync with the Op in PartialComponent::PartialComponent(PartialComponent&&).
  using Op = fruit::impl::meta::Apply<
      fruit::impl::meta::ComposeFunctors<
          fruit::impl::meta::ProcessDeferredBindings,
          fruit::impl::meta::ConvertComponent<DestComp>>,
      OtherComp>;
  (void)typename fruit::impl::meta::CheckIfError<typename Op::Result>::type();
}

inline Component<> createComponent() {
  return {};
}

template <typename Comp>
inline PartialComponent<Comp>::PartialComponent(fruit::impl::ComponentStorage&& storage)
  : storage(std::move(storage)) {
}

template <typename Comp>
template <typename SourceComp>
inline PartialComponent<Comp>::PartialComponent(PartialComponent<SourceComp> sourceComponent)
  : storage(std::move(sourceComponent.storage)) {
  // Keep in sync with the check in Component::Component(PartialComponent<OtherComp>).
  using Op = fruit::impl::meta::Apply<
      fruit::impl::meta::ComposeFunctors<
          fruit::impl::meta::ProcessDeferredBindings,
          fruit::impl::meta::ConvertComponent<Comp>>,
      SourceComp>;
  Op()(storage);
}

template <typename Comp>
template <typename AnnotatedI, typename AnnotatedC>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::meta::AddDeferredInterfaceBinding<AnnotatedI, AnnotatedC>, Comp>::Result>
PartialComponent<Comp>::bind() && {
  using Op = fruit::impl::meta::Apply<fruit::impl::meta::AddDeferredInterfaceBinding<AnnotatedI, AnnotatedC>, Comp>;
  (void)typename fruit::impl::meta::CheckIfError<typename Op::Result>::type();
  Op()(storage);
  return {std::move(storage)};
}

template <typename Comp>
template <typename AnnotatedSignature>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::meta::DeferredRegisterConstructor<AnnotatedSignature>, Comp>::Result>
PartialComponent<Comp>::registerConstructor() && {
  using Op = fruit::impl::meta::Apply<fruit::impl::meta::DeferredRegisterConstructor<AnnotatedSignature>, Comp>;
  (void)typename fruit::impl::meta::CheckIfError<typename Op::Result>::type();
  Op()(storage);
  return {std::move(storage)};
}

template <typename Comp>
template <typename C>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::meta::RegisterInstance<C, C>, Comp>::Result>
PartialComponent<Comp>::bindInstance(C& instance) && {
  using Op = fruit::impl::meta::Apply<fruit::impl::meta::RegisterInstance<C, C>, Comp>;
  (void)typename fruit::impl::meta::CheckIfError<typename Op::Result>::type();
  Op()(storage, instance);
  return {std::move(storage)};
}

template <typename Comp>
template <typename AnnotatedC, typename C>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::meta::RegisterInstance<AnnotatedC, C>, Comp>::Result>
PartialComponent<Comp>::bindInstance(C& instance) && {
  using Op = fruit::impl::meta::Apply<fruit::impl::meta::RegisterInstance<AnnotatedC, C>, Comp>;
  (void)typename fruit::impl::meta::CheckIfError<typename Op::Result>::type();
  Op()(storage, instance);
  return {std::move(storage)};
}

template <typename Comp>
template <typename Lambda>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::meta::DeferredRegisterProvider<Lambda>, Comp>::Result>
PartialComponent<Comp>::registerProvider(Lambda lambda) && {
  (void)lambda;
  using Op = fruit::impl::meta::Apply<fruit::impl::meta::DeferredRegisterProvider<Lambda>, Comp>;
  (void)typename fruit::impl::meta::CheckIfError<typename Op::Result>::type();
  Op()(storage);
  return {std::move(storage)};
}

template <typename Comp>
template <typename AnnotatedSignature, typename Lambda>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::meta::DeferredRegisterProviderWithAnnotations<AnnotatedSignature, Lambda>, Comp>::Result>
PartialComponent<Comp>::registerProvider(Lambda lambda) && {
  (void)lambda;
  using Op = fruit::impl::meta::Apply<fruit::impl::meta::DeferredRegisterProviderWithAnnotations<AnnotatedSignature, Lambda>, Comp>;
  (void)typename fruit::impl::meta::CheckIfError<typename Op::Result>::type();
  Op()(storage);
  return {std::move(storage)};
}

template <typename Comp>
template <typename AnnotatedI, typename AnnotatedC>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::meta::AddInterfaceMultibinding<AnnotatedI, AnnotatedC>, Comp>::Result>
PartialComponent<Comp>::addMultibinding() && {
  using Op = fruit::impl::meta::Apply<fruit::impl::meta::AddInterfaceMultibinding<AnnotatedI, AnnotatedC>, Comp>;
  (void)typename fruit::impl::meta::CheckIfError<typename Op::Result>::type();
  Op()(storage);
  return {std::move(storage)};
}

template <typename Comp>
template <typename C>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::meta::AddInstanceMultibinding<C, C>, Comp>::Result>
PartialComponent<Comp>::addInstanceMultibinding(C& instance) && {
  using Op = fruit::impl::meta::Apply<fruit::impl::meta::AddInstanceMultibinding<C, C>, Comp>;
  FruitStaticAssert(!fruit::impl::meta::Apply<fruit::impl::meta::IsError, typename Op::Result>::value, "");
  Op()(storage, instance);
  return {std::move(storage)};
}

template <typename Comp>
template <typename AnnotatedC, typename C>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::meta::AddInstanceMultibinding<AnnotatedC, C>, Comp>::Result>
PartialComponent<Comp>::addInstanceMultibinding(C& instance) && {
  using Op = fruit::impl::meta::Apply<fruit::impl::meta::AddInstanceMultibinding<AnnotatedC, C>, Comp>;
  FruitStaticAssert(!fruit::impl::meta::Apply<fruit::impl::meta::IsError, typename Op::Result>::value, "");
  Op()(storage, instance);
  return {std::move(storage)};
}

template <typename Comp>
template <typename C>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::meta::AddInstanceMultibindings<C, C>, Comp>::Result>
PartialComponent<Comp>::addInstanceMultibindings(std::vector<C>& instances) && {
  using Op = fruit::impl::meta::Apply<fruit::impl::meta::AddInstanceMultibindings<C, C>, Comp>;
  FruitStaticAssert(!fruit::impl::meta::Apply<fruit::impl::meta::IsError, typename Op::Result>::value, "");
  Op()(storage, instances);
  return {std::move(storage)};
}

template <typename Comp>
template <typename AnnotatedC, typename C>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::meta::AddInstanceMultibindings<AnnotatedC, C>, Comp>::Result>
PartialComponent<Comp>::addInstanceMultibindings(std::vector<C>& instances) && {
  using Op = fruit::impl::meta::Apply<fruit::impl::meta::AddInstanceMultibindings<AnnotatedC, C>, Comp>;
  FruitStaticAssert(!fruit::impl::meta::Apply<fruit::impl::meta::IsError, typename Op::Result>::value, "");
  Op()(storage, instances);
  return {std::move(storage)};
}

template <typename Comp>
template <typename Lambda>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::meta::RegisterMultibindingProvider<Lambda>, Comp>::Result>
PartialComponent<Comp>::addMultibindingProvider(Lambda lambda) && {
  (void)lambda;
  using Op = fruit::impl::meta::Apply<fruit::impl::meta::RegisterMultibindingProvider<Lambda>, Comp>;
  (void)typename fruit::impl::meta::CheckIfError<typename Op::Result>::type();
  Op()(storage);
  return {std::move(storage)};
}
  
template <typename Comp>
template <typename AnnotatedSignature, typename Lambda>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::meta::RegisterMultibindingProviderWithAnnotations<AnnotatedSignature, Lambda>, Comp>::Result>
PartialComponent<Comp>::addMultibindingProvider(Lambda lambda) && {
  (void)lambda;
  using Op = fruit::impl::meta::Apply<fruit::impl::meta::RegisterMultibindingProviderWithAnnotations<AnnotatedSignature, Lambda>, Comp>;
  (void)typename fruit::impl::meta::CheckIfError<typename Op::Result>::type();
  Op()(storage);
  return {std::move(storage)};
}
  
template <typename Comp>
template <typename DecoratedSignature, typename Lambda>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::meta::RegisterFactory<DecoratedSignature, Lambda>, Comp>::Result>
PartialComponent<Comp>::registerFactory(Lambda) && {
  using Op = fruit::impl::meta::Apply<fruit::impl::meta::RegisterFactory<DecoratedSignature, Lambda>, Comp>;
  (void)typename fruit::impl::meta::CheckIfError<typename Op::Result>::type();
  Op()(storage);
  return {std::move(storage)};
}

template <typename Comp>
template <typename... OtherCompParams>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::meta::InstallComponentHelper<OtherCompParams...>, Comp>::Result>
PartialComponent<Comp>::install(Component<OtherCompParams...> component) && {
  using Op = fruit::impl::meta::Apply<fruit::impl::meta::InstallComponentHelper<OtherCompParams...>, Comp>;
  (void)typename fruit::impl::meta::CheckIfError<typename Op::Result>::type();
  Op()(storage, std::move(component.storage));
  return {std::move(storage)};
}

} // namespace fruit


#endif // FRUIT_COMPONENT_DEFN_H
