/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_LABELS_APRIL_11_2016)
#define ELEMENTS_LABELS_APRIL_11_2016

#include <elements/element/element.hpp>
#include <elements/element/proxy.hpp>
#include <elements/element/traversal.hpp>
#include <elements/support/theme.hpp>
#include <elements/support/receiver.hpp>
#include <elements/support/text_reader_writer.hpp>
#include <artist/font.hpp>
#include <infra/string_view.hpp>
#include <string>

namespace cycfi::elements
{
   using artist::font_descr;

   /**
    * \struct default_label_styler
    *
    * \brief
    *    This struct encapsulates the styles of a default label.
    *
    *    `default_label_styler` provides the default look and feel for a
    *    label, such as font styles, sizes, colors, and alignments. The
    *    `get_*` member functions return these defaults. Various label
    *    stylers extend the defaults for different aspects of the label.
    */
   struct default_label_styler : element, text_reader_u8
   {
   public:

      using font_type = font_descr;
      using base_type = default_label_styler;

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      void                    enable(bool state = true) override;
      bool                    is_enabled() const override;

      // These member functions return the defaults

      virtual font_type       get_font() const;
      virtual float           get_font_size() const;
      virtual float           get_default_font_size() const;
      virtual color           get_font_color() const;
      virtual int             get_text_align() const;

   private:

      bool                    _is_enabled = true;
   };

   namespace concepts
   {
      template <typename T>
      concept LabelStyler = std::is_base_of_v<default_label_styler, std::decay_t<T>>;
   }

   /**
    * \struct basic_label_styler_base
    *
    * \brief
    *    Base for creating custom label stylers with supplied text.
    *
    * \tparam Base
    *    Base class fulfilling the LabelStyler concept.
    */
   template <concepts::LabelStyler Base>
   struct basic_label_styler_base : Base, text_writer_u8
   {
      using text_type = std::string_view;
      using base_type = basic_label_styler_base<typename Base::base_type>;

                              basic_label_styler_base(std::string text)
                               : _text(std::move(text))
                              {}

      text_type               get_text() const override ;
      void                    set_text(string_view text) override;

   private:

      std::string             _text;
   };

   /**
    * \struct label_styler_with_font
    *
    * \brief
    *    Label styler with custom font.
    *
    * \tparam Base
    *    Base class fulfilling the LabelStyler concept.
    */
   template <concepts::LabelStyler Base>
   struct label_styler_with_font : Base
   {
      using font_type = font_descr;
      using base_type = label_styler_with_font<typename Base::base_type>;

                              label_styler_with_font(Base base, font_type font_)
                               : Base(std::move(base)), _font(font_)
                              {}

      font_type               get_font() const override;
      void                    set_font(font_type font_);

   private:

      font_descr              _font;
   };

   /**
    * \struct label_styler_with_font_size
    *
    * \brief
    *    Label styler with custom font size.
    *
    * \tparam Base
    *    Base class fulfilling the LabelStyler concept.
    */
   template <concepts::LabelStyler Base>
   struct label_styler_with_font_size : Base
   {
      using base_type = label_styler_with_font_size<typename Base::base_type>;

                              label_styler_with_font_size(Base base, float size)
                               : Base(std::move(base)), _size(size)
                              {}

      float                   get_font_size() const override;
      void                    set_font_size(float size);
      void                    set_relative_font_size(float size);

   private:

      float                   _size;
   };

   /**
    * \struct label_styler_with_font_color
    *
    * \brief
    *    Label styler with custom font color.
    *
    * \tparam Base
    *    Base class fulfilling the LabelStyler concept.
    */
   template <concepts::LabelStyler Base>
   struct label_styler_with_font_color : Base
   {
      using base_type = label_styler_with_font_color<typename Base::base_type>;

                              label_styler_with_font_color(Base base, color color_)
                               : Base(std::move(base)), _color(color_)
                              {}

      color                   get_font_color() const override;
      void                    set_font_color(color color_);

   private:

      color                   _color;
   };

   /**
    * \struct label_with_text_align
    *
    * \brief
    *    Label styler with custom text alignment.
    *
    * \tparam Base
    *    Base class fulfilling the LabelStyler concept.
    */
   template <concepts::LabelStyler Base>
   struct label_with_text_align : Base
   {
      using base_type = label_with_text_align<typename Base::base_type>;

                              label_with_text_align(Base base, int align)
                               : Base(std::move(base)), _align(align)
                              {}

      int                     get_text_align() const override;
      void                    text_align(int align);

   private:

      int                     _align;
   };

   /**
    * \struct label_styler_gen
    *
    * \brief
    *    Zero-overhead abstraction for generating label stylers with various
    *    properties.
    *
    *    `label_styler_gen` generates label stylers with custom fonts, font
    *    sizes, font colors, text alignments, and more.
    *
    * @details
    *    Starting from a basic label, various generator member functions can
    *    be cascaded to provide various custom properties, as needed. For
    *    example:
    *
    * \code{.cpp}
    *     label("My Label")
    *       .font(font_descr{"Open Sans"}.semi_bold())
    *       .font_color(colors::antique_white)
    *       .font_size(18)
    * \endcode
    *
    *    Only the properties specified take up memory space.
    *
    * \tparam Base
    *    A base class fulfilling the LabelStyler concept.
    */
   template <concepts::LabelStyler Base>
   struct label_styler_gen : Base
   {
      using Base::Base;
      using base_type = typename Base::base_type;

      using font_type      = font_descr;
      using gen_font       = label_styler_gen<label_styler_with_font<base_type>>;
      using gen_font_size  = label_styler_gen<label_styler_with_font_size<base_type>>;
      using gen_font_color = label_styler_gen<label_styler_with_font_color<base_type>>;
      using gen_text_align = label_styler_gen<label_with_text_align<base_type>>;

      gen_font                font(font_type font_) const;
      gen_font_size           font_size(float size) const;
      gen_font_size           relative_font_size(float size) const;
      gen_font_color          font_color(color color_) const;
      gen_text_align          text_align(int align) const;
   };

   using basic_label = basic_label_styler_base<default_label_styler>;
   using label = label_styler_gen<basic_label>;

   /**
    * \struct default_heading
    *
    * \brief
    *    Default label styler for headings.
    */
   struct default_heading : default_label_styler
   {
      using font_type = font_descr;
      using base_type = default_heading;

      font_type               get_font() const override;
      float                   get_font_size() const override;
      float                   get_default_font_size() const override;
      color                   get_font_color() const override;
      int                     get_text_align() const override;
   };

   using basic_heading = basic_label_styler_base<default_heading>;
   using heading = label_styler_gen<basic_heading>;

   /**
    * \class as_label_element
    *
    * \brief
    *    As label receives a value of type T and converts it to the label's
    *    text using a user-supplied function.
    *
    * \tparam Subject
    *    The Subject of the label that must fulfill the `Element` concept.
    *
    * \tparam T
    *    The type of value that is converted to the label's text.
    *
    * \tparam F
    *    The type of user-defined function that is used for conversion of the
    *    value into text.
    */
   template <concepts::Element Subject, typename T, typename F>
   class as_label_element : public receiver<T>, public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;
      using param_type = typename receiver<T>::param_type;
      using getter_type = typename receiver<T>::getter_type;

                              as_label_element(F as_string, Subject label);

      void                    value(param_type val) override;
      getter_type             value() const override;

   private:

      F                       _as_string;
      T                       _value;
   };

   template <typename T, concepts::Element Subject, typename F>
   inline as_label_element<remove_cvref_t<Subject>, T, remove_cvref_t<F>>
   as_label(F&& as_string, Subject&& subject);

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   /**
    * \brief
    *    Get the font for labels from the theme.
    *
    * @return
    *    Default font description for labels.
    */
   inline default_label_styler::font_type default_label_styler::get_font() const
   {
      return get_theme().label_font;
   }

   /**
    * \brief
    *    Get the size of the font for labels from the theme.
    *
    * @return
    *    Default size of the font for labels.
    */
   inline float default_label_styler::get_font_size() const
   {
      return get_theme().label_font._size;
   }

   /**
    * \brief
    *    Get the default size of the font for labels from the theme.
    *
    * @return
    *    Default size of the font for labels.
    */
   inline float default_label_styler::get_default_font_size() const
   {
      return get_theme().label_font._size;
   }

   /**
    * \brief
    *    Get the color of the font for labels from the theme.
    *
    * @return
    *    Default color of the font for labels.
    */
   inline color default_label_styler::get_font_color() const
   {
      return get_theme().label_font_color;
   }

   /**
    * \brief
    *    Get the text alignment for labels from the theme.
    *
    * @return
    *    Default text alignment for labels.
    */
   inline int default_label_styler::get_text_align() const
   {
      return get_theme().label_text_align;
   }

   /**
    * \brief
    *    Get the font of headings from the theme.
    *
    * @return
    *    Default font description for headings.
    */
   inline default_heading::font_type default_heading::get_font() const
   {
      return get_theme().heading_font;
   }

   /**
    * \brief
    *    Get the size of the font for headings from the theme.
    *
    * @return
    *    Default size of the font for headings.
    */
   inline float default_heading::get_font_size() const
   {
      return get_theme().heading_font._size;
   }

   /**
    * \brief
    *    Get the default size of the font for headings from the theme.
    *
    * @return
    *    Default size of the font for headings.
    */
   inline float default_heading::get_default_font_size() const
   {
      return get_theme().heading_font._size;
   }

   /**
    * \brief
    *    Get the color of the font for headings from the theme.
    *
    * @return
    *    Default color of the font for headings.
    */
   inline color default_heading::get_font_color() const
   {
      return get_theme().heading_font_color;
   }

   /**
    * \brief
    *    Get the text alignment for headings from the theme.
    *
    * @return
    *    Default text alignment for headings.
    */
   inline int default_heading::get_text_align() const
   {
      return get_theme().heading_text_align;
   }

   /**
    * \brief
    *    Generator for setting the font of a label.
    *
    * \tparam Base
    *    A base class that fulfills the `LabelStyler` concept.
    *
    * \param font_
    *    The font to be set.
    *
    * @return
    *    `label_styler_gen` with the specified font.
    */
   template <concepts::LabelStyler Base>
   inline typename label_styler_gen<Base>::gen_font
   label_styler_gen<Base>::font(font_type font_) const
   {
      return {*this, font_};
   }

   /**
    * \brief
    *    Generator for setting the font size of a label.
    *
    * \tparam Base
    *    A base class that fulfills the `LabelStyler` concept.
    *
    * \param size
    *    The size of the font to be set.
    *
    * @return
    *    `label_styler_gen` with the specified font size.
    */
   template <concepts::LabelStyler Base>
   inline typename label_styler_gen<Base>::gen_font_size
   label_styler_gen<Base>::font_size(float size) const
   {
      return {*this, size};
   }

   /**
    * \brief
    *    Generator for setting the relative font size of a label.
    *
    * \tparam Base
    *    A base class that fulfills the `LabelStyler` concept.
    *
    * \param size
    *    The relative size of the font to be set.
    *
    * @return
    *    `label_styler_gen` with the specified relative font size.
    */
   template <concepts::LabelStyler Base>
   inline typename label_styler_gen<Base>::gen_font_size
   label_styler_gen<Base>::relative_font_size(float size) const
   {
      return {*this, Base::get_default_font_size() * size};
   }

   /**
    * \brief
    *    Generator for setting the font color of a label.
    *
    * \tparam Base
    *    A base class that fulfills the `LabelStyler` concept.
    *
    * \param color_
    *    The color of the font to be set.
    *
    * @return
    *    `label_styler_gen` with the specified font color.
    */
   template <concepts::LabelStyler Base>
   inline typename label_styler_gen<Base>::gen_font_color
   label_styler_gen<Base>::font_color(color color_) const
   {
      return {*this, color_};
   }

   /**
    * \brief
    *    Generator for setting the text alignment of a label.
    *
    * \tparam Base
    *    A base class that fulfills the `LabelStyler` concept.
    *
    * \param align
    *    The alignment to be set.
    *
    * @return
    *    `label_styler_gen` with the specified text alignment.
    */
   template <concepts::LabelStyler Base>
   inline typename label_styler_gen<Base>::gen_text_align
   label_styler_gen<Base>::text_align(int align) const
   {
      return {*this, align};
   }

   /**
    * \brief
    *    Construct a new `as_label_element`.
    *
    * \tparam Subject
    *    A type that fulfills the `Element` concept.
    *
    * \tparam T
    *    The type of value that is converted to the label's text.
    *
    * \tparam F
    *    The type of user-defined function that is used for conversion of the
    *    value into text.
    *
    * \param as_string
    *    The function to convert the value into the label's text.
    *
    * \param subject
    *    The subject.
    */
   template <concepts::Element Subject, typename T, typename F>
   inline as_label_element<Subject, T, F>::as_label_element(F as_string, Subject subject)
    : _as_string(as_string)
    , base_type(std::move(subject))
   {}

   /**
    * \brief
    *    Sets the value for the label.
    *
    * \tparam Subject
    *    A type that fulfills the `Element` concept.
    *
    * \tparam T
    *    The type of value that is converted to the label's text.
    *
    * \tparam F
    *    The type of user-defined function that is used for conversion of the
    *    value into text.
    *
    * \param val
    *    The value to be set.
    */
   template <concepts::Element Subject, typename T, typename F>
   inline void as_label_element<Subject, T, F>::value(param_type val)
   {
      _value = val;
      if (auto tw = find_subject<text_writer_u8*>(this))
         tw->set_text(_as_string(val));
   }

   /**
    * \brief
    *    Get the value of the label.
    *
    * \tparam Subject
    *    A type that fulfills the `Element` concept.
    *
    * \tparam T
    *    The type of value that is converted to the label's text.
    *
    * \tparam F
    *    The type of user-defined function that is used for conversion of the
    *    value into text.
    *
    * @return
    *    The value of the label.
    */
   template <concepts::Element Subject, typename T, typename F>
   inline typename as_label_element<Subject, T, F>::getter_type
   as_label_element<Subject, T, F>::value() const
   {
      return _value;
   }

   /**
    * \brief
    *    Construct a new `as_label_element`.
    *
    * \tparam T
    *    The type of value that is converted to the label's text.
    *
    * \tparam Subject
    *    A type that fulfills the `Element` concept.
    *
    * \tparam F
    *    The type of user-defined function that is used for conversion of the
    *    value into text.
    *
    * \param as_string
    *    The function to convert the value into the label's text.
    *
    * \param subject
    *    The subject.
    *
    * @return
    *    A new `as_label_element` given Subject and as_string Function.
    */
   template <typename T, concepts::Element Subject, typename F>
   inline as_label_element<remove_cvref_t<Subject>, T, remove_cvref_t<F>>
   as_label(F&& as_string, Subject&& subject)
   {
      return {std::forward<F>(as_string), std::forward<Subject>(subject)};
   }

   /**
    * \brief
    *    Get the text of the label.
    *
    * \tparam Base
    *    A base class that fulfills the `LabelStyler` concept.
    *
    * @return
    *    The text of the label.
    */
   template <concepts::LabelStyler Base>
   inline typename basic_label_styler_base<Base>::text_type
   basic_label_styler_base<Base>::get_text() const
   {
      return _text;
   }

   /**
    * \brief
    *    Set the text of the label.
    *
    * \tparam Base
    *    A base class that fulfills the `LabelStyler` concept.
    *
    * \param text
    *    The text to be set.
    */
   template <concepts::LabelStyler Base>
   inline void basic_label_styler_base<Base>::set_text(string_view text)
   {
      _text = std::string(text);
   }

   /**
    * \brief
    *    Get the font of the label.
    *
    * \tparam Base
    *    A base class that fulfills the `LabelStyler` concept.
    *
    * @return The font of the label.
    */
   template <concepts::LabelStyler Base>
   inline typename label_styler_with_font<Base>::font_type
   label_styler_with_font<Base>::get_font() const
   {
      return _font;
   }

   /**
    * \brief
    *    Set the font of the label.
    *
    * \tparam Base
    *    A base class that fulfills the `LabelStyler` concept.
    *
    * \param font_
    *    The font to be set.
    */
   template <concepts::LabelStyler Base>
   inline void label_styler_with_font<Base>::set_font(font_type font_)
   {
      _font = font_;
   }

   /**
    * \brief
    *    Get the font size of the label.
    *
    * \tparam Base
    *    A base class that fulfills the `LabelStyler` concept.
    *
    * @return
    *    The font size of the label.
    */
   template <concepts::LabelStyler Base>
   inline float label_styler_with_font_size<Base>::get_font_size() const
   {
      return _size;
   }

   /**
    * \brief
    *    Set the font size of the label.
    *
    * \tparam Base
    *    A base class that fulfills the `LabelStyler` concept.
    *
    * \param size
    *    The font size to be set.
    */
   template <concepts::LabelStyler Base>
   inline void label_styler_with_font_size<Base>::set_font_size(float size)
   {
      _size = size;
   }

   /**
    * \brief
    *    Set the relative font size of the label.
    *
    * \tparam Base
    *    A base class that fulfills the `LabelStyler` concept.
    *
    * \param size
    *    The relative font size to be set.
    */
   template <concepts::LabelStyler Base>
   inline void label_styler_with_font_size<Base>::set_relative_font_size(float size)
   {
      _size = Base::get_default_font_size() * size;
   }

   /**
    * \brief
    *    Get the font color of the label.
    *
    * \tparam Base
    *    A base class that fulfills the `LabelStyler` concept.
    *
    * @return
    *    The font color of the label.
    */
   template <concepts::LabelStyler Base>
   inline color label_styler_with_font_color<Base>::get_font_color() const
   {
      return _color;
   }

   /**
    * \brief
    *    Set the font color of the label.
    *
    * \tparam Base
    *    A base class that fulfills the `LabelStyler` concept.
    *
    * \param color_
    *    The font color to be set.
    */
   template <concepts::LabelStyler Base>
   inline void label_styler_with_font_color<Base>::set_font_color(color color_)
   {
      _color = color_;
   }

   /**
    * \brief
    *    Get the text alignment of the label.
    *
    * \tparam Base
    *    A base class that fulfills the `LabelStyler` concept.
    *
    * @return
    *    The text alignment of the label.
    */
   template <concepts::LabelStyler Base>
   inline int label_with_text_align<Base>::get_text_align() const
   {
      return _align;
   }

   /**
    * \brief
    *    Set the text alignment of the label.
    *
    * \tparam Base
    *    A base class that fulfills the `LabelStyler` concept.
    *
    * \param align
    *    The text alignment to be set.
    */
   template <concepts::LabelStyler Base>
   inline void label_with_text_align<Base>::text_align(int align)
   {
      _align = align;
   }
}

#endif
