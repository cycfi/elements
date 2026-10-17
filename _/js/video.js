// Videos that autoplay also play inline: phones play a video in the page,
// rather than full screen, only when it says so.
document.addEventListener('DOMContentLoaded', function () {
  document.querySelectorAll('video[autoplay]').forEach(function (v) {
    v.setAttribute('playsinline', '')
    v.muted = true
    v.play().catch(function () {})
  })
})
