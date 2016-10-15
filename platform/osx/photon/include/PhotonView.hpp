/*=================================================================================================
   Copyright © 2016 Cycfi Research. All rights reserved.

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_VIEW_AUGUST_20_2016)
#define PHOTON_VIEW_AUGUST_20_2016

#import <Cocoa/Cocoa.h>
#include <photon/view.hpp>
#include <photon/support/key.hpp>
#include <map>

using key_map = std::map<photon::key_code, photon::key_action>;

@interface PhotonView : NSView <NSTextInputClient>
{
   photon::view                  _view;
   NSTrackingArea*               _tracking_area;
   NSMutableAttributedString*    _marked_text;
   key_map                       _keys;
   bool                          _first_time;
}

@end

// Internal Cocoa-only API
namespace photon
{
   key_code    translate_key(unsigned int key);
   int         translate_flags(NSUInteger flags);
   NSUInteger  translate_key_to_modifier_flag(key_code key);
}

#endif