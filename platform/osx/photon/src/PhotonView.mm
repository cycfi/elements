/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#import "PhotonView.hpp"

namespace photon
{
   class platform_access
   {
   public:

      static void init_view(view& v, PhotonView* self_)
      {
         v._impl = (__bridge view_impl*) self_;
      }
   };
};

@implementation PhotonView

- (void) awakeFromNib
{
   photon::platform_access::init_view(_view, self);
}

- (void) dealloc
{
}

-(BOOL) isFlipped
{
   return YES;
}

- (void) drawRect:(NSRect)dirty
{
   [super drawRect:dirty];

   _view.draw();
}

@end
