#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define a template class NamedType,
///        that implements the abstract base class template INamedType.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include <cstddef>
#include <string_view>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  using DataID = size_t;  ///< The type for the unique id of Named Type implementations

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  /// Helper macro to declare a template instantiation of the class template NamedType,
  /// and give this template instance name.
  /// The template parameter SEMANTIC of the class template NamedType will be defined as "struct <NAME>___t".
  ///
  /// Example:
  /// \code{.cpp}
  /// DECLARE_NAMED_TYPE(Force, double);    // This defines the named type Force, which stores its value as a double.
  /// DECLARE_NAMED_TYPE(Current, double);  // This defines the named type Current, which stores its value as a double.
  /// struct Rectangle
  /// {
  ///   int width;
  ///   int height;
  /// };
  /// DECLARE_NAMED_TYPE(MyParams, struct Rectangle);  // This defines the named type MyParams, which stores its value as a Rectangle.
  ///
  /// IContainer* container = new Force(2.5);
  /// auto asCurrent  = Current::safeCast(container); // asCurrent will be a Current* pointer type, and its value is null.
  /// auto asForce    = Force::safeCast(container);   // asForce will be a Force* pointer type, and its value will be a valid pointer to object *container.
  /// \endcode
  /// 
  /// @param[in] NAME       The type name that will be defined for the named type template instantiation.
  ///                       This macro defines a type alias with this name in the surrounding scope.
  /// @param[in] BASETYPE   The actual storage data type for this named data type.
#ifdef WIN32
  #define DECLARE_NAMED_TYPE(NAME, BASETYPE) \
    extern template class ::comm::axisprofile::NamedType<BASETYPE, struct NAME ## ___t>;\
    using NAME = ::comm::axisprofile::NamedType<BASETYPE, struct NAME ## ___t>
#else
  #define DECLARE_NAMED_TYPE(NAME, BASETYPE) \
    using NAME = ::comm::axisprofile::NamedType<BASETYPE, struct NAME ## ___t>
#endif

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  /// @brief Generic base class for Named Type classes.
  class IContainer
  {
  public: // methods
    /// Destructor
    virtual ~IContainer() noexcept = default;

    /// @brief get the unique type id of the object.
    ///
    /// Overrides should ensure that the returned id uniquely identifies the object
    /// its named type, such that if two IContainer objects return the same DataID value,
    /// they both should be considered to be of the same semantic type.
    /// 
    /// @return        static id that uniquely identifies the Named Type
    virtual DataID getID() const = 0;
  };

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief Non-abstract base class, used to implement named types.
  ///
  /// This class implements the pure-virtual methods of IContainer.
  /// 
  /// @tparam BASETYPE    The type for the underlaying stored data/value.
  ///                     An object of this NamedType class represents data that is stored as a BASETYPE object.
  ///                     BASETYPE could be a simple type (e.g. int, double, enum) or a struct/class type.
  /// @tparam SEMANTIC    A type (can be incomplete) to further specialize the template instantiation,
  ///                     to fully specify a unique named type.
  template <typename BASETYPE, typename SEMANTIC>
  class NamedType : public IContainer
  {
  public: // methods

    /// @brief Default constructor
    NamedType() = default;

    /// @brief Constructor with initial value
    /// @param[in]  value   initial value of the container
    explicit NamedType(BASETYPE const& value): m_value(value) {}

    /// @brief Constructor with initial value as an r-value
    /// @param[in]  value   the value that is moved to this container
    explicit NamedType(BASETYPE&& value): m_value(std::move(value)) {}

    /// @brief Copy constructor
    NamedType(const NamedType&) = default;

    /// @brief Move constructor
    NamedType(NamedType&&) noexcept = default;

    /// @brief destructor
    virtual ~NamedType() noexcept override = default;

    /// @brief Assignment operator
    /// @return reference of this object
    NamedType& operator=(const NamedType&) = default;

    /// @brief Move operator
    /// @return reference of this object
    NamedType& operator=(NamedType&&) noexcept = default;

    /// @brief Get value reference
    /// @return value reference
    BASETYPE& getRef()
    {
      return m_value;
    }

    /// @brief Get value const reference
    /// @return  const value reference
    BASETYPE const& get() const
    {
      return m_value;
    }

    /// @brief Set value
    /// @param[in]  value   new value to set
    void set(BASETYPE value)
    {
      m_value = value;
    }

    /// @brief Get this NamedType class its unique (static) id
    DataID getID() const override
    {
      return NamedType<BASETYPE, SEMANTIC>::getMyID();
    }

    /// @brief  Safely casts a generic IContainer to this NamedType, if its id matches this class its static id.
    /// @param[in]  container   An IContainer object to cast to this named type class.
    /// @return   A const pointer of this class type to the container, if this class its
    ///           static type id (@ref getMyID) matches the given container its type id.
    ///           Null pointer if the type ids do not match.
    static const NamedType* safeCast(const IContainer* container)
    {
      if (!container || (container->getID() != NamedType<BASETYPE, SEMANTIC>::getMyID()))
      {
        return nullptr;
      }

      return static_cast<const NamedType<BASETYPE, SEMANTIC>*>(container);
    }

    /// @brief Safely casts a generic IContainer to this NamedType, if its id matches this class its static id.
    /// @param[in]  container   An IContainer object to cast to this named type class.
    /// @return   A non-const pointer of this class type to the container, if this class its
    ///           static type id (@ref getMyID) matches the given container its type id.
    ///           Null pointer if the type ids do not match.
    static NamedType* safeCast(IContainer* container)
    {
      return const_cast<NamedType<BASETYPE, SEMANTIC>*>(safeCast(const_cast<const IContainer*>(container)));
    }

    /// @brief Get this NamedType class its unique (static) id
    /// @return The id for this NamedType class
    static DataID getMyID()
    {
    #ifdef WIN32
      static size_t id {std::hash<std::string_view>{}(std::string_view(__FUNCSIG__))};
    #else
      static size_t id {std::hash<std::string_view>{}(std::string_view(__PRETTY_FUNCTION__))};
    #endif
      return id;
    }

  private: // data

    /// @brief The stored value itself
    BASETYPE m_value {};
  };
}
