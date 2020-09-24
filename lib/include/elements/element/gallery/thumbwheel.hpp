/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_THUMBWHEEL_SEPTEMBER_23_2020)
#define ELEMENTS_GALLERY_THUMBWHEEL_SEPTEMBER_23_2020

#include <elements/element/thumbwheel.hpp>
#include <elements/element/dynamic_list.hpp>
#include <elements/element/port.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Basic (vertical and horizontal) thumbwheels (You can use these as the
   // subject of thumbwheel)
   //
   //    A thumbwheel subject can be constructed using a vport or hport (see
   //    port.hpp) and a receiver<double> which recieves a 0.0 to 1.0 value
   //    that determines the vertical or horizontal position of the content.
   //
   //    The thumbwheel is a continuous device. If you need the value to be
   //    quantized, you can pass a non-zero value to the constructor. For
   //    example, a quantize value of 0.25 will quantize the possible values
   //    to 0.0, 0.25, 0.5, 0.75 and 1.0.
   ////////////////////////////////////////////////////////////////////////////
   class basic_thumbwheel_element : public basic_receiver<double>
   {
   public:

      basic_thumbwheel_element(float quantize_ = 0.0)
       : _quantize(quantize_)
      {}

      virtual void         align(double val) = 0;
      virtual double       align() const = 0;
      void                 value(double val) override;

   protected:

      float                quantize() const { return _quantize; }
      void                 make_aligner(context const& ctx);
      void                 do_align(view& view_, rect const& bounds, double val);

   private:

      using align_function = std::function<void(double val)>;

      float                _quantize;
      align_function       _aligner;
   };

   struct basic_vthumbwheel_element : vport_element, basic_thumbwheel_element
   {
      using basic_thumbwheel_element::basic_thumbwheel_element;

      view_limits          limits(basic_context const& ctx) const override;
      void                 draw(context const& ctx) override;
      void                 align(double val) override;
      double               align() const override;
   };

   struct basic_hthumbwheel_element : hport_element, basic_thumbwheel_element
   {
      using basic_thumbwheel_element::basic_thumbwheel_element;

      view_limits          limits(basic_context const& ctx) const override;
      void                 draw(context const& ctx) override;
      void                 align(double val) override;
      double               align() const override;
   };

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, basic_vthumbwheel_element>
   basic_vthumbwheel(Subject&& subject, double quantize_ = 0.0)
   {
      return { std::forward<Subject>(subject), quantize_ };
   }

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, basic_hthumbwheel_element>
   basic_hthumbwheel(Subject&& subject, double quantize_ = 0.0)
   {
      return { std::forward<Subject>(subject), quantize_ };
   }

   ////////////////////////////////////////////////////////////////////////////
   // This function utilizes a dynamic_list and the basic_vthumbwheel to
   // present items in a vertically oriented thumbwheel, allowing it to
   // scroll into the items in the list that is dynamically created given the
   // (user supplied) compose function.
   ////////////////////////////////////////////////////////////////////////////
   template <typename F>
   inline auto vthumbwheel(std::size_t num_items, F&& compose)
   {
      using ftype = remove_cvref_t<F>;
      auto body = basic_vthumbwheel(
         dynamic_list{ basic_cell_composer(num_items, std::forward<ftype>(compose)) }
       , 1.0 / (num_items-1)
      );
      return thumbwheel(std::move(body), { 0.0f, 0.0f });
   }
}}

#endif
