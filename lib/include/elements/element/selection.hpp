/*=================================================================================================
   Copyright (c) 2016-2024 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=================================================================================================*/
#if !defined(ELEMENTS_SELECTION_OCTOBER_19_2019)
#define ELEMENTS_SELECTION_OCTOBER_19_2019

#include <elements/element/proxy.hpp>
#include <vector>
#include <functional>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // The selectable is a pure abstract base class for selectable elements such as menu items and
   // radio buttons and elements in a list with user selectable items.
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class selectable
   {
   public:

      virtual                 ~selectable() = default;
      virtual bool            is_selected() const = 0;
      virtual void            select(bool state) = 0;
   };

   //==============================================================================================
   /** \class selection_list
     *
     * The selection_list is a proxy base class that manages a container subject with selectable
     * items. The selection_list manages singe or multiple selections, click select, shift-click
     * select, shift-control (command on MacOS), up and down navigation also and with shift and
     * control (command on MacOS) selection extension.
     */
   //==============================================================================================
   class selection_list_element : public proxy_base
   {
   public:

      using indices_type = std::vector<std::size_t>;
      using on_select_function = std::function<void(int select_start, int select_end)>;
      using base_type = proxy_base;

                              selection_list_element(bool multi_select = true);

      bool                    click(context const& ctx, mouse_button btn) override;
      bool                    key(context const& ctx, key_info k) override;
      bool                    wants_control() const override { return true; }
      bool                    wants_focus() const override { return true; }

      indices_type            get_selection() const;
      void                    set_selection(indices_type const& selection);
      void                    update_selection(int start, int end);
      int                     get_select_start() const;
      int                     get_select_end() const;

      void                    select_all();
      void                    select_none();

      on_select_function      on_select = [](int, int){};

   private:

      bool                    _multi_select = false;
      int                     _select_start = -1;
      int                     _select_end = -1;
   };

   //==============================================================================================
   /** \brief Make a selection list given a subject.
     *
     * \param subject The subject of the selection list proxy. The subject is expected to contain
     *                a composite. This requirement is not enforced. The selection_list simply
     *                does nothing if the subject does not contain a composite to work on.
     *
     * \param multi_select Determines if we allow multiple selections or not. Defaults to `true`.
     */
   //==============================================================================================
   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, selection_list_element>
   selection_list(Subject&& subject, bool multi_select = true)
   {
      return {std::forward<Subject>(subject), multi_select};
   }

   //==============================================================================================
   // Inlines
   //==============================================================================================

   /**
    * \brief Construct a selection_list_element.
    * \param multi_select Determines if we allow multiple selections or not. Defaults to `true`.
   */
   inline selection_list_element::selection_list_element(bool multi_select)
    : _multi_select{multi_select}
   {}
}

#endif

