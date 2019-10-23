/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_SIZE_APRIL_14_2016)
#define ELEMENTS_SIZE_APRIL_14_2016

#include <elements/element/element.hpp>
#include <memory>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Fixed sizing elements
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class size_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              size_element(point size, Subject&& subject);

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);

   private:

      point          _size;
   };

   template <typename Subject>
   inline size_element<Subject>::size_element(point size, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _size(size)
   {}

   template <typename Subject>
   inline size_element<Subject>
   fixed_size(point size, Subject&& subject)
   {
      return { size, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline view_limits size_element<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float size_x = _size.x;
      float size_y = _size.y;
      clamp(size_x, e_limits.min.x, e_limits.max.x);
      clamp(size_y, e_limits.min.y, e_limits.max.y);
      return { { size_x, size_y }, { size_x, size_y } };
   }

   template <typename Subject>
   inline void size_element<Subject>::prepare_subject(context& ctx)
   {
      ctx.bounds.right = ctx.bounds.left + _size.x;
      ctx.bounds.bottom = ctx.bounds.top + _size.y;
   }

   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class hsize_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              hsize_element(float width, Subject&& subject);

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);

   private:

      float                   _width;
   };

   template <typename Subject>
   inline hsize_element<Subject>::hsize_element(float width, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _width(width)
   {}

   template <typename Subject>
   inline hsize_element<Subject>
   hsize(float width, Subject&& subject)
   {
      return { width, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline view_limits hsize_element<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float width = _width;
      clamp(width, e_limits.min.x, e_limits.max.x);
      return { { width, e_limits.min.y }, { width, e_limits.max.y } };
   }

   template <typename Subject>
   inline void hsize_element<Subject>::prepare_subject(context& ctx)
   {
      ctx.bounds.right = ctx.bounds.left + _width;
   }

   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class vsize_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              vsize_element(float height, Subject&& subject);

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);

   private:

      float                   _height;
   };

   template <typename Subject>
   inline vsize_element<Subject>::vsize_element(float height, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _height(height)
   {}

   template <typename Subject>
   inline vsize_element<Subject>
   vsize(float height, Subject&& subject)
   {
      return { height, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline view_limits vsize_element<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float height = _height;
      clamp(height, e_limits.min.y, e_limits.max.y);
      return { { e_limits.min.x, height }, { e_limits.max.x, height } };
   }

   template <typename Subject>
   inline void vsize_element<Subject>::prepare_subject(context& ctx)
   {
      ctx.bounds.bottom = ctx.bounds.top + _height;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Minumum sizing elements
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class min_size_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              min_size_element(point size, Subject&& subject);

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);

   private:

      point                   _size;
   };

   template <typename Subject>
   inline min_size_element<Subject>::min_size_element(point size, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _size(size)
   {}

   template <typename Subject>
   inline min_size_element<Subject>
   min_size(point size, Subject&& subject)
   {
      return { size, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline view_limits min_size_element<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float size_x = _size.x;
      float size_y = _size.y;
      clamp(size_x, e_limits.min.x, e_limits.max.x);
      clamp(size_y, e_limits.min.y, e_limits.max.y);
      return { { size_x, size_y }, { e_limits.max.x, e_limits.max.y } };
   }

   template <typename Subject>
   inline void min_size_element<Subject>::prepare_subject(context& ctx)
   {
      if (ctx.bounds.width() < _size.x)
         ctx.bounds.width(_size.x);
      if (ctx.bounds.height() < _size.y)
         ctx.bounds.height(_size.x);
   }

   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class hmin_size_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              hmin_size_element(float width, Subject&& subject);

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);

   private:

      float                   _width;
   };

   template <typename Subject>
   inline hmin_size_element<Subject>::hmin_size_element(float width, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _width(width)
   {}

   template <typename Subject>
   inline hmin_size_element<Subject>
   hmin_size(float width, Subject&& subject)
   {
      return { width, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline view_limits hmin_size_element<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float width = _width;
      clamp(width, e_limits.min.x, e_limits.max.x);
      return { { width, e_limits.min.y }, e_limits.max };
   }

   template <typename Subject>
   inline void hmin_size_element<Subject>::prepare_subject(context& ctx)
   {
      if (ctx.bounds.width() < _width)
         ctx.bounds.width(_width);
   }

   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class vmin_size_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              vmin_size_element(float height, Subject&& subject);

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);

   private:

      float                   _height;
   };

   template <typename Subject>
   inline vmin_size_element<Subject>::vmin_size_element(float height, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _height(height)
   {}

   template <typename Subject>
   inline vmin_size_element<Subject>
   vmin_size(float height, Subject&& subject)
   {
      return { height, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline view_limits vmin_size_element<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float height = _height;
      clamp(height, e_limits.min.y, e_limits.max.y);
      return { { e_limits.min.x, height }, e_limits.max };
   }

   template <typename Subject>
   inline void vmin_size_element<Subject>::prepare_subject(context& ctx)
   {
      if (ctx.bounds.height() < _height)
         ctx.bounds.height(_height);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Maximum sizing elements
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class max_size_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              max_size_element(point size, Subject&& subject);

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);

   private:

      point                   _size;
   };

   template <typename Subject>
   inline max_size_element<Subject>::max_size_element(point size, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _size(size)
   {}

   template <typename Subject>
   inline max_size_element<Subject>
   max_size(point size, Subject&& subject)
   {
      return { size, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline view_limits max_size_element<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float size_x = _size.x;
      float size_y = _size.y;
      clamp(size_x, e_limits.min.x, e_limits.max.x);
      clamp(size_y, e_limits.min.y, e_limits.max.y);
      return { { e_limits.min.x, e_limits.min.y }, { size_x, size_y } };
   }

   template <typename Subject>
   inline void max_size_element<Subject>::prepare_subject(context& ctx)
   {
      if (ctx.bounds.width() > _size.x)
         ctx.bounds.width(_size.x);
      if (ctx.bounds.height() > _size.y)
         ctx.bounds.height(_size.x);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Stretch elements
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class hstretch_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              hstretch_element(float stretch, Subject&& subject);

      virtual view_stretch    stretch() const;

   private:

      float                   _stretch;
   };

   template <typename Subject>
   inline hstretch_element<Subject>::hstretch_element(float stretch, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _stretch(stretch)
   {}

   template <typename Subject>
   inline view_stretch hstretch_element<Subject>::stretch() const
   {
      return { _stretch, this->subject().stretch().y };
   }

   template <typename Subject>
   inline hstretch_element<Subject>
   hstretch(float stretch, Subject&& subject)
   {
      return { stretch, std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class vstretch_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              vstretch_element(float stretch, Subject&& subject);

      virtual view_stretch    stretch() const;

   private:

      float                   _stretch;
   };

   template <typename Subject>
   inline vstretch_element<Subject>::vstretch_element(float stretch, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _stretch(stretch)
   {}

   template <typename Subject>
   inline view_stretch vstretch_element<Subject>::stretch() const
   {
      return { this->subject().stretch().x, _stretch };
   }

   template <typename Subject>
   inline vstretch_element<Subject>
   vstretch(float stretch, Subject&& subject)
   {
      return { stretch, std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Size limited
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   struct limit_element : public proxy<Subject>
   {
      using base_type = proxy<Subject>;

                              limit_element(view_limits limits_, Subject&& subject)
                               : base_type(std::forward<Subject>(subject))
                               , _limits(limits_)
                              {}

      virtual view_limits     limits(basic_context const& ctx) const;

      view_limits             _limits;
   };

   template <typename Subject>
   inline limit_element<Subject>
   limit(view_limits limits_, Subject&& subject)
   {
      return { limits_, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline view_limits
   limit_element<Subject>::limits(basic_context const& ctx) const
   {
      auto l = this->subject().limits(ctx);
      clamp_min(l.min.x, _limits.min.x);
      clamp_min(l.min.y, _limits.min.y);
      clamp_max(l.max.x, _limits.max.x);
      clamp_max(l.max.y, _limits.max.y);
      return l;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Scaled
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   struct scale_element : public proxy<Subject>
   {
      using base_type = proxy<Subject>;

                              scale_element(point scale_, Subject&& subject)
                               : base_type(std::forward<Subject>(subject))
                               , _scale(scale_)
                              {}

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual view_stretch    stretch() const;
      virtual void            prepare_subject(context& ctx);
      virtual void            prepare_subject(context& ctx, point& p);
      virtual void            restore_subject(context& ctx);

      point                   _scale;
   };

   template <typename Subject>
   inline scale_element<Subject>
   scale(point scale_, Subject&& subject)
   {
      return { scale_, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline view_limits
   scale_element<Subject>::limits(basic_context const& ctx) const
   {
      auto l = this->subject().limits(ctx);
      l.min.x *= _scale.x;
      l.min.y *= _scale.y;
      l.max.x *= _scale.x;
      l.max.y *= _scale.y;
      clamp_max(l.max.x, full_extent);
      clamp_max(l.max.y, full_extent);
      return l;
   }

   template <typename Subject>
   inline view_stretch
   scale_element<Subject>::stretch() const
   {
      auto s = this->subject().stretch();
      return { s.x * _scale.x, s.y * _scale.y };
   }

   template <typename Subject>
   inline void scale_element<Subject>::prepare_subject(context& ctx)
   {
      auto& canvas_ = ctx.canvas;
      canvas_.save();
      auto& bounds = ctx.bounds;
      canvas_.scale(_scale);
      auto  offset = point{ bounds.left / _scale.x, bounds.top / _scale.y };
      bounds = {
         offset.x
       , offset.y
       , offset.x + (bounds.width() / _scale.x)
       , offset.y + (bounds.height() / _scale.y)
      };
   }

   template <typename Subject>
   void scale_element<Subject>::prepare_subject(context& ctx, point& p)
   {
      prepare_subject(ctx);
      p = ctx.canvas.device_to_user(p);
   }

   template <typename Subject>
   inline void scale_element<Subject>::restore_subject(context& ctx)
   {
      ctx.canvas.restore();
   }
}}

#endif
