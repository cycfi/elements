/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_SIZE_APRIL_14_2016)
#define ELEMENTS_SIZE_APRIL_14_2016

#include <elements/element/element.hpp>
#include <elements/element/proxy.hpp>
#include <infra/support.hpp>
#include <memory>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Fixed sizing elements
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class size_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              size_element(point size, Subject subject);

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

      void                    fixed_size(point size) { _size = size; }
      point                   fixed_size() const { return _size; }

   private:

      point          _size;
   };

   template <concepts::Element Subject>
   inline size_element<Subject>::size_element(point size, Subject subject)
    : base_type(std::move(subject))
    , _size(size)
   {}

   template <concepts::Element Subject>
   inline size_element<remove_cvref_t<Subject>>
   fixed_size(point size, Subject&& subject)
   {
      return {size, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline view_limits size_element<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float size_x = _size.x;
      float size_y = _size.y;
      clamp(size_x, e_limits.min.x, e_limits.max.x);
      clamp(size_y, e_limits.min.y, e_limits.max.y);
      return {{size_x, size_y}, {size_x, size_y}};
   }

   template <concepts::Element Subject>
   inline void size_element<Subject>::prepare_subject(context& ctx)
   {
      // use the fixed width/height as computed by limits
      auto e_limits = this->limits(ctx);
      ctx.bounds.right = ctx.bounds.left + e_limits.min.x;
      ctx.bounds.bottom = ctx.bounds.top + e_limits.min.y;
   }

   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class hsize_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              hsize_element(float width, Subject subject);

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

      void                    hsize(float width) { _width = width; }
      float                   hsize() const { return _width; }

   private:

      float                   _width;
   };

   template <concepts::Element Subject>
   inline hsize_element<Subject>::hsize_element(float width, Subject subject)
    : base_type(std::move(subject))
    , _width(width)
   {}

   template <concepts::Element Subject>
   inline hsize_element<remove_cvref_t<Subject>>
   hsize(float width, Subject&& subject)
   {
      return {width, std::forward<Subject>(subject)};
   }

   inline hsize_element<element>
   hspace(float width)
   {
      return {width, {}};
   }

   template <concepts::Element Subject>
   inline view_limits hsize_element<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float width = _width;
      clamp(width, e_limits.min.x, e_limits.max.x);
      return {{width, e_limits.min.y}, {width, e_limits.max.y}};
   }

   template <concepts::Element Subject>
   inline void hsize_element<Subject>::prepare_subject(context& ctx)
   {
      // use the fixed width as computed by limits
      auto e_limits = this->limits(ctx);
      ctx.bounds.right = ctx.bounds.left + e_limits.min.x;
   }

   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class vsize_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              vsize_element(float height, Subject subject);

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

      void                    vsize(float height) { _height = height; }
      float                   vsize() const { return _height; }

   private:

      float                   _height;
   };

   template <concepts::Element Subject>
   inline vsize_element<Subject>::vsize_element(float height, Subject subject)
    : base_type(std::move(subject))
    , _height(height)
   {}

   template <concepts::Element Subject>
   inline vsize_element<remove_cvref_t<Subject>>
   vsize(float height, Subject&& subject)
   {
      return {height, std::forward<Subject>(subject)};
   }

   inline vsize_element<element>
   vspace(float height)
   {
      return {height, {}};
   }

   template <concepts::Element Subject>
   inline view_limits vsize_element<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float height = _height;
      clamp(height, e_limits.min.y, e_limits.max.y);
      return {{e_limits.min.x, height}, {e_limits.max.x, height}};
   }

   template <concepts::Element Subject>
   inline void vsize_element<Subject>::prepare_subject(context& ctx)
   {
      // use the fixed height as computed by limits
      auto e_limits = this->limits(ctx);
      ctx.bounds.bottom = ctx.bounds.top + e_limits.min.y;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Minumum sizing elements
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class min_size_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              min_size_element(point size, Subject subject);

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

      void                    min_size(point size) { _size = size; }
      point                   min_size() const { return _size; }

   private:

      point                   _size;
   };

   template <concepts::Element Subject>
   inline min_size_element<Subject>::min_size_element(point size, Subject subject)
    : base_type(std::move(subject))
    , _size(size)
   {}

   template <concepts::Element Subject>
   inline min_size_element<remove_cvref_t<Subject>>
   min_size(point size, Subject&& subject)
   {
      return {size, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline view_limits min_size_element<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float size_x = _size.x;
      float size_y = _size.y;
      clamp(size_x, e_limits.min.x, e_limits.max.x);
      clamp(size_y, e_limits.min.y, e_limits.max.y);
      return {{size_x, size_y}, {e_limits.max.x, e_limits.max.y}};
   }

   template <concepts::Element Subject>
   inline void min_size_element<Subject>::prepare_subject(context& ctx)
   {
      if (ctx.bounds.width() < _size.x)
         ctx.bounds.width(_size.x);
      if (ctx.bounds.height() < _size.y)
         ctx.bounds.height(_size.x);
   }

   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class hmin_size_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              hmin_size_element(float width, Subject subject);

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

      void                    hmin_size(float width) { _size = width; }
      float                   hmin_size() const { return _size; }

   private:

      float                   _size;
   };

   template <concepts::Element Subject>
   inline hmin_size_element<Subject>::hmin_size_element(float size, Subject subject)
    : base_type(std::move(subject))
    , _size(size)
   {}

   template <concepts::Element Subject>
   inline hmin_size_element<remove_cvref_t<Subject>>
   hmin_size(float width, Subject&& subject)
   {
      return {width, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline hmin_size_element<remove_cvref_t<Subject>>
   hmin_pad(float pad, Subject&& subject)
   {
      return {-pad, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline view_limits hmin_size_element<Subject>::limits(basic_context const& ctx) const
   {
      auto e_limits = this->subject().limits(ctx);
      float width = _size > 0? _size : e_limits.min.x + -_size;
      clamp(width, e_limits.min.x, e_limits.max.x);
      return {{width, e_limits.min.y}, e_limits.max};
   }

   template <concepts::Element Subject>
   inline void hmin_size_element<Subject>::prepare_subject(context& ctx)
   {
      if (ctx.bounds.width() < _size)
         ctx.bounds.width(_size);
   }

   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class hmin_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              hmin_element(Subject subject);
      view_limits             limits(basic_context const& ctx) const override;
   };

   template <concepts::Element Subject>
   inline hmin_element<Subject>::hmin_element(Subject subject)
    : base_type(std::move(subject))
   {}

   template <concepts::Element Subject>
   inline hmin_element<remove_cvref_t<Subject>>
   hmin(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline view_limits hmin_element<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      return {e_limits.min, {e_limits.min.x, e_limits.max.y}};
   }

   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class vmin_size_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              vmin_size_element(float height, Subject subject);

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

      void                    vmin_size(float height) { _height = height; }
      float                   vmin_size() const { return _height; }

   private:

      float                   _height;
   };

   template <concepts::Element Subject>
   inline vmin_size_element<Subject>::vmin_size_element(float height, Subject subject)
    : base_type(std::move(subject))
    , _height(height)
   {}

   template <concepts::Element Subject>
   inline vmin_size_element<remove_cvref_t<Subject>>
   vmin_size(float height, Subject&& subject)
   {
      return {height, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline view_limits vmin_size_element<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float height = _height;
      clamp(height, e_limits.min.y, e_limits.max.y);
      return {{e_limits.min.x, height}, e_limits.max};
   }

   template <concepts::Element Subject>
   inline void vmin_size_element<Subject>::prepare_subject(context& ctx)
   {
      if (ctx.bounds.height() < _height)
         ctx.bounds.height(_height);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Maximum sizing elements
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class max_size_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              max_size_element(point size, Subject subject);

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

      void                    max_size(point size) { _size = size; }
      point                   max_size() const { return _size; }

   private:

      point                   _size;
   };

   template <concepts::Element Subject>
   inline max_size_element<Subject>::max_size_element(point size, Subject subject)
    : base_type(std::move(subject))
    , _size(size)
   {}

   template <concepts::Element Subject>
   inline max_size_element<remove_cvref_t<Subject>>
   max_size(point size, Subject&& subject)
   {
      return {size, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline view_limits max_size_element<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float size_x = _size.x;
      float size_y = _size.y;
      clamp(size_x, e_limits.min.x, e_limits.max.x);
      clamp(size_y, e_limits.min.y, e_limits.max.y);
      return {{e_limits.min.x, e_limits.min.y}, {size_x, size_y}};
   }

   template <concepts::Element Subject>
   inline void max_size_element<Subject>::prepare_subject(context& ctx)
   {
      if (ctx.bounds.width() > _size.x)
         ctx.bounds.width(_size.x);
      if (ctx.bounds.height() > _size.y)
         ctx.bounds.height(_size.x);
   }

   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class hmax_size_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              hmax_size_element(float size, Subject subject);

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

      void                    hmax_size(float size) { _size = size; }
      float                   hmax_size() const { return _size; }

   private:

      float                   _size;
   };

   template <concepts::Element Subject>
   inline hmax_size_element<Subject>::hmax_size_element(float size, Subject subject)
    : base_type(std::move(subject))
    , _size(size)
   {}

   template <concepts::Element Subject>
   inline hmax_size_element<remove_cvref_t<Subject>>
   hmax_size(float size, Subject&& subject)
   {
      return {size, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline view_limits hmax_size_element<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float size_x = _size;
      clamp(size_x, e_limits.min.x, e_limits.max.x);
      return {{e_limits.min.x, e_limits.min.y}, {size_x, e_limits.max.y}};
   }

   template <concepts::Element Subject>
   inline void hmax_size_element<Subject>::prepare_subject(context& ctx)
   {
      if (ctx.bounds.width() > _size)
         ctx.bounds.width(_size);
   }

   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class vmax_size_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              vmax_size_element(float size, Subject subject);

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

      void                    vmax_size(float size) { _size = size; }
      float                   vmax_size() const { return _size; }

   private:

      float                   _size;
   };

   template <concepts::Element Subject>
   inline vmax_size_element<Subject>::vmax_size_element(float size, Subject subject)
    : base_type(std::move(subject))
    , _size(size)
   {}

   template <concepts::Element Subject>
   inline vmax_size_element<remove_cvref_t<Subject>>
   vmax_size(float size, Subject&& subject)
   {
      return {size, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline view_limits vmax_size_element<Subject>::limits(basic_context const& ctx) const
   {
      auto  e_limits = this->subject().limits(ctx);
      float size_y = _size;
      clamp(size_y, e_limits.min.y, e_limits.max.y);
      return {{e_limits.min.x, e_limits.min.y}, {size_y, e_limits.max.y}};
   }

   template <concepts::Element Subject>
   inline void vmax_size_element<Subject>::prepare_subject(context& ctx)
   {
      if (ctx.bounds.height() > _size)
         ctx.bounds.height(_size);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Stretch elements
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class hstretch_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              hstretch_element(float stretch, Subject subject);

      view_stretch            stretch() const override;

      void                    hstretch(float stretch) { _stretch = stretch; }
      float                   hstretch() const { return _stretch; }

   private:

      float                   _stretch;
   };

   template <concepts::Element Subject>
   inline hstretch_element<Subject>::hstretch_element(float stretch, Subject subject)
    : base_type(std::move(subject))
    , _stretch(stretch)
   {}

   template <concepts::Element Subject>
   inline view_stretch hstretch_element<Subject>::stretch() const
   {
      return {_stretch, this->subject().stretch().y};
   }

   template <concepts::Element Subject>
   inline hstretch_element<remove_cvref_t<Subject>>
   hstretch(float stretch, Subject&& subject)
   {
      return {stretch, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline auto
   no_hstretch(Subject&& subject)
   {
      return hstretch(0.0f, std::forward<Subject>(subject));
   }

   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class vstretch_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              vstretch_element(float stretch, Subject subject);

      view_stretch            stretch() const override;

      void                    vstretch(float stretch) { _stretch = stretch; }
      float                   vstretch() const { return _stretch; }

   private:

      float                   _stretch;
   };

   template <concepts::Element Subject>
   inline vstretch_element<Subject>::vstretch_element(float stretch, Subject subject)
    : base_type(std::move(subject))
    , _stretch(stretch)
   {}

   template <concepts::Element Subject>
   inline view_stretch vstretch_element<Subject>::stretch() const
   {
      return {this->subject().stretch().x, _stretch};
   }

   template <concepts::Element Subject>
   inline vstretch_element<remove_cvref_t<Subject>>
   vstretch(float stretch, Subject&& subject)
   {
      return {stretch, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline auto
   no_vstretch(Subject&& subject)
   {
      return vstretch(0.0f, std::forward<Subject>(subject));
   }

   ////////////////////////////////////////////////////////////////////////////
   // Span elements
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class span_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              span_element(unsigned span, Subject subject);

      unsigned                span() const override   { return _span; }
      void                    span(float span)        { _span = span; }

   private:

      unsigned                _span;
   };

   template <concepts::Element Subject>
   inline span_element<Subject>::span_element(unsigned span, Subject subject)
    : base_type(std::move(subject))
    , _span(span)
   {}

   template <concepts::Element Subject>
   inline span_element<remove_cvref_t<Subject>>
   span(unsigned span, Subject&& subject)
   {
      return {span, std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Size limited
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class limit_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              limit_element(view_limits limits_, Subject subject)
                               : base_type(std::move(subject))
                               , _limits(limits_)
                              {}

      view_limits             limits(basic_context const& ctx) const override;

      void                    limit(view_limits limits) { _limits = limits; }
      view_limits             limit() const { return _limits; }

   private:

      view_limits             _limits;
   };

   template <concepts::Element Subject>
   inline limit_element<remove_cvref_t<Subject>>
   limit(view_limits limits_, Subject&& subject)
   {
      return {limits_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
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
   template <concepts::Element Subject>
   class scale_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              scale_element(float scale_, Subject subject)
                               : base_type(std::move(subject))
                               , _scale(scale_)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      view_stretch            stretch() const override;
      void                    prepare_subject(context& ctx) override;
      void                    prepare_subject(context& ctx, point& p) override;
      void                    restore_subject(context& ctx) override;

      void                    scale(float scale_) { _scale = scale_; }
      float                   scale() const { return _scale; }

   private:

      float                   _scale;
   };

   template <concepts::Element Subject>
   inline scale_element<remove_cvref_t<Subject>>
   scale(float scale_, Subject&& subject)
   {
      return {scale_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline view_limits
   scale_element<Subject>::limits(basic_context const& ctx) const
   {
      auto l = this->subject().limits(ctx);
      l.min.x *= _scale;
      l.min.y *= _scale;
      l.max.x *= _scale;
      l.max.y *= _scale;
      clamp_max(l.max.x, full_extent);
      clamp_max(l.max.y, full_extent);
      return l;
   }

   template <concepts::Element Subject>
   inline view_stretch
   scale_element<Subject>::stretch() const
   {
      auto s = this->subject().stretch();
      return {s.x * _scale, s.y * _scale};
   }

   template <concepts::Element Subject>
   inline void scale_element<Subject>::prepare_subject(context& ctx)
   {
      auto& canvas_ = ctx.canvas;
      canvas_.save();
      canvas_.scale({_scale, _scale});
      ctx.bounds = device_to_user(ctx.bounds, ctx.canvas);
   }

   template <concepts::Element Subject>
   void scale_element<Subject>::prepare_subject(context& ctx, point& p)
   {
      prepare_subject(ctx);
      p = ctx.canvas.device_to_user(p);
   }

   template <concepts::Element Subject>
   inline void scale_element<Subject>::restore_subject(context& ctx)
   {
      ctx.canvas.restore();
   }

   ////////////////////////////////////////////////////////////////////////////
   // Collapsible
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class hcollapsible_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;
      using is_collapsed_function = std::function<bool()>;

                              hcollapsible_element(Subject subject);
      view_limits             limits(basic_context const& ctx) const override;
      is_collapsed_function   is_collapsed = []{ return false; };
   };

   template <concepts::Element Subject>
   inline hcollapsible_element<Subject>::hcollapsible_element(Subject subject)
    : base_type(std::move(subject))
   {}

   template <concepts::Element Subject>
   inline view_limits hcollapsible_element<Subject>::limits(basic_context const& ctx) const
   {
      auto e_limits = this->subject().limits(ctx);
      if (is_collapsed())
         e_limits.min.x = e_limits.max.x = 0;
      return e_limits;
   }

   template <concepts::Element Subject>
   inline hcollapsible_element<remove_cvref_t<Subject>>
   hcollapsible(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class vcollapsible_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;
      using is_collapsed_function = std::function<bool()>;

                              vcollapsible_element(Subject subject);
      view_limits             limits(basic_context const& ctx) const override;
      is_collapsed_function   is_collapsed = []{ return false; };
   };

   template <concepts::Element Subject>
   inline vcollapsible_element<Subject>::vcollapsible_element(Subject subject)
    : base_type(std::move(subject))
   {}

   template <concepts::Element Subject>
   inline view_limits vcollapsible_element<Subject>::limits(basic_context const& ctx) const
   {
      auto e_limits = this->subject().limits(ctx);
      if (is_collapsed())
         e_limits.min.y = e_limits.max.y = 0;
      return e_limits;
   }

   template <concepts::Element Subject>
   inline vcollapsible_element<remove_cvref_t<Subject>>
   vcollapsible(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }
}

#endif
