/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_SIZE_APRIL_14_2016)
#define PHOTON_GUI_LIB_WIDGET_SIZE_APRIL_14_2016

#include <photon/widget/widget.hpp>
#include <memory>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Fixed sizing widgets
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class size_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                     size_widget(point size, Subject&& subject);
                     size_widget(point size, Subject const& subject);

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   prepare_subject(context& ctx);

   private:

      point          _size;
   };

   template <typename Subject>
   inline size_widget<Subject>::size_widget(point size, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _size(size)
   {}

   template <typename Subject>
   inline size_widget<Subject>::size_widget(point size, Subject const& subject)
    : base_type(subject)
    , _size(size)
   {}

   template <typename Subject>
   inline size_widget<typename std::decay<Subject>::type>
   fixed_size(point size, Subject&& subject)
   {
      return { size, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline rect size_widget<Subject>::limits(basic_context const& ctx) const
   {
      rect  e_limits = this->subject().limits(ctx);
      float size_x = _size.x;
      float size_y = _size.y;
      clamp(size_x, e_limits.left, e_limits.right);
      clamp(size_y, e_limits.top, e_limits.bottom);
      return rect{ size_x, size_y, size_x, size_y };
   }

   template <typename Subject>
   inline void size_widget<Subject>::prepare_subject(context& ctx)
   {
      ctx.bounds.right = ctx.bounds.left + _size.x;
      ctx.bounds.bottom = ctx.bounds.top + _size.y;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class hsize_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                     hsize_widget(float width, Subject&& subject);
                     hsize_widget(float width, Subject const& subject);

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   prepare_subject(context& ctx);

   private:

      float          _width;
   };

   template <typename Subject>
   inline hsize_widget<Subject>::hsize_widget(float width, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _width(width)
   {}

   template <typename Subject>
   inline hsize_widget<Subject>::hsize_widget(float width, Subject const& subject)
    : base_type(subject)
    , _width(width)
   {}

   template <typename Subject>
   inline hsize_widget<typename std::decay<Subject>::type>
   hsize(float width, Subject&& subject)
   {
      return { width, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline rect hsize_widget<Subject>::limits(basic_context const& ctx) const
   {
      rect  e_limits = this->subject().limits(ctx);
      float width = _width;
      clamp(width, e_limits.left, e_limits.right);
      return rect{ width, e_limits.top, width, e_limits.bottom };
   }

   template <typename Subject>
   inline void hsize_widget<Subject>::prepare_subject(context& ctx)
   {
      ctx.bounds.right = ctx.bounds.left + _width;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class vsize_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                     vsize_widget(float height, Subject&& subject);
                     vsize_widget(float height, Subject const& subject);

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   prepare_subject(context& ctx);

   private:

      float          _height;
   };

   template <typename Subject>
   inline vsize_widget<Subject>::vsize_widget(float height, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _height(height)
   {}

   template <typename Subject>
   inline vsize_widget<Subject>::vsize_widget(float height, Subject const& subject)
    : base_type(subject)
    , _height(height)
   {}

   template <typename Subject>
   inline vsize_widget<typename std::decay<Subject>::type>
   vsize(float height, Subject&& subject)
   {
      return { height, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline rect vsize_widget<Subject>::limits(basic_context const& ctx) const
   {
      rect  e_limits = this->subject().limits(ctx);
      float height = _height;
      clamp(height, e_limits.top, e_limits.bottom);
      return rect{ e_limits.left, height, e_limits.right, height };
   }

   template <typename Subject>
   inline void vsize_widget<Subject>::prepare_subject(context& ctx)
   {
      ctx.bounds.bottom = ctx.bounds.top + _height;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Minumum sizing widgets
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class min_size_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                     min_size_widget(point size, Subject&& subject);
                     min_size_widget(point size, Subject const& subject);

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   prepare_subject(context& ctx);

   private:

      point          _size;
   };

   template <typename Subject>
   inline min_size_widget<Subject>::min_size_widget(point size, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _size(size)
   {}

   template <typename Subject>
   inline min_size_widget<Subject>::min_size_widget(point size, Subject const& subject)
    : base_type(subject)
    , _size(size)
   {}

   template <typename Subject>
   inline min_size_widget<typename std::decay<Subject>::type>
   min_size(point size, Subject&& subject)
   {
      return { size, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline rect min_size_widget<Subject>::limits(basic_context const& ctx) const
   {
      rect  e_limits = this->subject().limits(ctx);
      float size_x = _size.x;
      float size_y = _size.y;
      clamp(size_x, e_limits.left, e_limits.right);
      clamp(size_y, e_limits.top, e_limits.bottom);
      return rect{ size_x, size_y, e_limits.right, e_limits.bottom };
   }

   template <typename Subject>
   inline void min_size_widget<Subject>::prepare_subject(context& ctx)
   {
      if (ctx.bounds.width() < _size.x)
         ctx.bounds.width(_size.x);
      if (ctx.bounds.height() < _size.y)
         ctx.bounds.height(_size.x);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class hmin_size_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                     hmin_size_widget(float width, Subject&& subject);
                     hmin_size_widget(float width, Subject const& subject);

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   prepare_subject(context& ctx);

   private:

      float          _width;
   };

   template <typename Subject>
   inline hmin_size_widget<Subject>::hmin_size_widget(float width, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _width(width)
   {}

   template <typename Subject>
   inline hmin_size_widget<Subject>::hmin_size_widget(float width, Subject const& subject)
    : base_type(subject)
    , _width(width)
   {}

   template <typename Subject>
   inline hmin_size_widget<typename std::decay<Subject>::type>
   hmin_size(float width, Subject&& subject)
   {
      return { width, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline rect hmin_size_widget<Subject>::limits(basic_context const& ctx) const
   {
      rect  e_limits = this->subject().limits(ctx);
      float width = _width;
      clamp(width, e_limits.left, e_limits.right);
      return rect{ width, e_limits.top, e_limits.right, e_limits.bottom };
   }

   template <typename Subject>
   inline void hmin_size_widget<Subject>::prepare_subject(context& ctx)
   {
      if (ctx.bounds.width() < _width)
         ctx.bounds.width(_width);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class vmin_size_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                     vmin_size_widget(float height, Subject&& subject);
                     vmin_size_widget(float height, Subject const& subject);

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   prepare_subject(context& ctx);

   private:

      float          _height;
   };

   template <typename Subject>
   inline vmin_size_widget<Subject>::vmin_size_widget(float height, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _height(height)
   {}

   template <typename Subject>
   inline vmin_size_widget<Subject>::vmin_size_widget(float height, Subject const& subject)
    : base_type(subject)
    , _height(height)
   {}

   template <typename Subject>
   inline vmin_size_widget<typename std::decay<Subject>::type>
   vmin_size(float height, Subject&& subject)
   {
      return { height, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline rect vmin_size_widget<Subject>::limits(basic_context const& ctx) const
   {
      rect  e_limits = this->subject().limits(ctx);
      float height = _height;
      clamp(height, e_limits.top, e_limits.bottom);
      return rect{ e_limits.left, height, e_limits.right, e_limits.bottom };
   }

   template <typename Subject>
   inline void vmin_size_widget<Subject>::prepare_subject(context& ctx)
   {
      if (ctx.bounds.height() < _height)
         ctx.bounds.height(_height);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Percentage sizing widgets
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class hpercent_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                     hpercent_widget(float percent, Subject&& subject);
                     hpercent_widget(float percent, Subject const& subject);

      virtual rect   limits(basic_context const& ctx) const;

   private:

      float          _percent;
   };

   template <typename Subject>
   inline hpercent_widget<Subject>::hpercent_widget(float percent, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _percent(percent)
   {}

   template <typename Subject>
   inline hpercent_widget<Subject>::hpercent_widget(float percent, Subject const& subject)
    : base_type(subject)
    , _percent(percent)
   {}

   template <typename Subject>
   inline hpercent_widget<typename std::decay<Subject>::type>
   hpercent(float percent, Subject&& subject)
   {
      return { percent, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline rect hpercent_widget<Subject>::limits(basic_context const& ctx) const
   {
      rect  e_limits = this->subject().limits(ctx);
      float max_width = std::max(e_limits.left, e_limits.right * _percent);
      return rect{ e_limits.left, e_limits.top, max_width, e_limits.bottom };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class vpercent_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                     vpercent_widget(float percent, Subject&& subject);
                     vpercent_widget(float percent, Subject const& subject);

      virtual rect   limits(basic_context const& ctx) const;

   private:

      float          _percent;
   };

   template <typename Subject>
   inline vpercent_widget<Subject>::vpercent_widget(float percent, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _percent(percent)
   {}

   template <typename Subject>
   inline vpercent_widget<Subject>::vpercent_widget(float percent, Subject const& subject)
    : base_type(subject)
    , _percent(percent)
   {}

   template <typename Subject>
   inline vpercent_widget<typename std::decay<Subject>::type>
   vpercent(float percent, Subject&& subject)
   {
      return { percent, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline rect vpercent_widget<Subject>::limits(basic_context const& ctx) const
   {
      rect  e_limits = this->subject().limits(ctx);
      float max_height = std::max(e_limits.top, e_limits.bottom * _percent);
      return rect{ e_limits.left, e_limits.top, e_limits.right, max_height };
   }
}

#endif