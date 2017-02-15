/*=======================================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_SIZE_APRIL_14_2016)
#define PHOTON_GUI_LIB_WIDGET_SIZE_APRIL_14_2016

#include <photon/widget/widget.hpp>
#include <memory>

namespace photon
{
   //////////////////////////////////////////////////////////////////////////////////////
   // Fixed sizing widgets
   //////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class size_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              size_widget(point size, Subject&& subject);
                              size_widget(point size, Subject const& subject);

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);

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
   inline widget_limits size_widget<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float size_x = _size.x;
      float size_y = _size.y;
      clamp(size_x, e_limits.min.x, e_limits.max.x);
      clamp(size_y, e_limits.min.y, e_limits.max.y);
      return { { size_x, size_y }, { size_x, size_y } };
   }

   template <typename Subject>
   inline void size_widget<Subject>::prepare_subject(context& ctx)
   {
      ctx.bounds.right = ctx.bounds.left + _size.x;
      ctx.bounds.bottom = ctx.bounds.top + _size.y;
   }

   //////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class hsize_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              hsize_widget(float width, Subject&& subject);
                              hsize_widget(float width, Subject const& subject);

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);

   private:

      float                   _width;
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
   inline widget_limits hsize_widget<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float width = _width;
      clamp(width, e_limits.min.x, e_limits.max.x);
      return { { width, e_limits.min.y }, { width, e_limits.max.y } };
   }

   template <typename Subject>
   inline void hsize_widget<Subject>::prepare_subject(context& ctx)
   {
      ctx.bounds.right = ctx.bounds.left + _width;
   }

   //////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class vsize_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              vsize_widget(float height, Subject&& subject);
                              vsize_widget(float height, Subject const& subject);

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);

   private:

      float                   _height;
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
   inline widget_limits vsize_widget<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float height = _height;
      clamp(height, e_limits.min.y, e_limits.max.y);
      return { { e_limits.min.x, height }, { e_limits.max.x, height } };
   }

   template <typename Subject>
   inline void vsize_widget<Subject>::prepare_subject(context& ctx)
   {
      ctx.bounds.bottom = ctx.bounds.top + _height;
   }

   //////////////////////////////////////////////////////////////////////////////////////
   // Minumum sizing widgets
   //////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class min_size_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              min_size_widget(point size, Subject&& subject);
                              min_size_widget(point size, Subject const& subject);

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);

   private:

      point                   _size;
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
   inline widget_limits min_size_widget<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float size_x = _size.x;
      float size_y = _size.y;
      clamp(size_x, e_limits.left, e_limits.right);
      clamp(size_y, e_limits.top, e_limits.bottom);
      return { size_x, size_y, e_limits.max.x, e_limits.max.y };
   }

   template <typename Subject>
   inline void min_size_widget<Subject>::prepare_subject(context& ctx)
   {
      if (ctx.bounds.width() < _size.x)
         ctx.bounds.width(_size.x);
      if (ctx.bounds.height() < _size.y)
         ctx.bounds.height(_size.x);
   }

   //////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class hmin_size_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              hmin_size_widget(float width, Subject&& subject);
                              hmin_size_widget(float width, Subject const& subject);

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);

   private:

      float                   _width;
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
   inline widget_limits hmin_size_widget<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float width = _width;
      clamp(width, e_limits.min.x, e_limits.max.x);
      return { width, e_limits.min.y, e_limits.max.x, e_limits.max.y };
   }

   template <typename Subject>
   inline void hmin_size_widget<Subject>::prepare_subject(context& ctx)
   {
      if (ctx.bounds.width() < _width)
         ctx.bounds.width(_width);
   }

   //////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class vmin_size_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              vmin_size_widget(float height, Subject&& subject);
                              vmin_size_widget(float height, Subject const& subject);

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);

   private:

      float                   _height;
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
   inline widget_limits vmin_size_widget<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float height = _height;
      clamp(height, e_limits.min.y, e_limits.max.y);
      return { e_limits.min.x, height, e_limits.max.x, e_limits.max.y };
   }

   template <typename Subject>
   inline void vmin_size_widget<Subject>::prepare_subject(context& ctx)
   {
      if (ctx.bounds.height() < _height)
         ctx.bounds.height(_height);
   }

   //////////////////////////////////////////////////////////////////////////////////////
   // Span sizing widgets
   //////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class hspan_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              hspan_widget(float span, Subject&& subject);
                              hspan_widget(float span, Subject const& subject);

      virtual widget_limits   limits(basic_context const& ctx) const;

   private:

      float                   _span;
   };

   template <typename Subject>
   inline hspan_widget<Subject>::hspan_widget(float span, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _span(span)
   {}

   template <typename Subject>
   inline hspan_widget<Subject>::hspan_widget(float span, Subject const& subject)
    : base_type(subject)
    , _span(span)
   {}

   template <typename Subject>
   inline hspan_widget<typename std::decay<Subject>::type>
   hspan(float span, Subject&& subject)
   {
      return { span, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline widget_limits hspan_widget<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float max_width = std::max(e_limits.min.x, e_limits.max.x * _span);
      return { { e_limits.min.x, e_limits.min.y }, { max_width, e_limits.max.y } };
   }

   //////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class vspan_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              vspan_widget(float span, Subject&& subject);
                              vspan_widget(float span, Subject const& subject);

      virtual widget_limits   limits(basic_context const& ctx) const;

   private:

      float                   _span;
   };

   template <typename Subject>
   inline vspan_widget<Subject>::vspan_widget(float span, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _span(span)
   {}

   template <typename Subject>
   inline vspan_widget<Subject>::vspan_widget(float span, Subject const& subject)
    : base_type(subject)
    , _span(span)
   {}

   template <typename Subject>
   inline vspan_widget<typename std::decay<Subject>::type>
   vspan(float span, Subject&& subject)
   {
      return { span, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline widget_limits vspan_widget<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float max_height = std::max(e_limits.min.y, e_limits.max.y * _span);
      return { { e_limits.min.x, e_limits.min.y }, { e_limits.max.x, max_height } };
   }

   //////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class fit_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              fit_widget(point size, Subject&& subject);
                              fit_widget(point size, Subject const& subject);

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);
      virtual void            prepare_subject(context& ctx, point& p);
      virtual void            restore_subject(context& ctx);

   private:

      point                   _size;
   };

   template <typename Subject>
   inline fit_widget<Subject>::fit_widget(point size, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _size(size)
   {}

   template <typename Subject>
   inline fit_widget<Subject>::fit_widget(point size, Subject const& subject)
    : base_type(subject)
    , _size(size)
   {}

   template <typename Subject>
   inline fit_widget<typename std::decay<Subject>::type>
   fit_scaled(point size, Subject&& subject)
   {
      return { size, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline widget_limits fit_widget<Subject>::limits(basic_context const& ctx) const
   {
      return { _size, { full_extent, full_extent } };
   }

   template <typename Subject>
   inline void fit_widget<Subject>::prepare_subject(context& ctx)
   {
      auto& canvas_ = ctx.canvas;
      canvas_.save();
      auto& bounds = ctx.bounds;
      auto  w = bounds.width();
      auto  h = bounds.height();
      auto  scale = point{ w/_size.x, h/_size.y };
      canvas_.scale(scale);
      auto  offset = point{ bounds.left / scale.x, bounds.top / scale.y };
      bounds = { offset.x, offset.y, offset.x + _size.x, offset.y + _size.y };
   }

   template <typename Subject>
   void fit_widget<Subject>::prepare_subject(context& ctx, point& p)
   {
      prepare_subject(ctx);
      p = ctx.canvas.device_to_user(p);
   }

   template <typename Subject>
   inline void fit_widget<Subject>::restore_subject(context& ctx)
   {
      ctx.canvas.restore();
   }
}

#endif