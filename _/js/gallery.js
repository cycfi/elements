// The gallery's carousels: arrows, dots, and the arrow keys move a carousel
// a slide at a time, and only the video in view plays. Without this script a
// carousel still scrolls sideways.
document.addEventListener('DOMContentLoaded', function () {
  document.querySelectorAll('.carousel').forEach(function (carousel) {
    var track = carousel.querySelector(':scope > .content')
    var slides = track.querySelectorAll(':scope > .imageblock, :scope > .videoblock')
    if (!slides.length) return

    var current = function () {
      return Math.round(track.scrollLeft / track.clientWidth)
    }
    var go = function (i) {
      i = (i + slides.length) % slides.length
      track.scrollTo({ left: i * track.clientWidth })
    }

    var button = function (cls, label, text, step) {
      var b = document.createElement('button')
      b.className = 'carousel-button ' + cls
      b.setAttribute('aria-label', label)
      b.textContent = text
      b.addEventListener('click', function () { go(current() + step) })
      carousel.appendChild(b)
    }
    button('carousel-prev', 'Previous', '‹', -1)
    button('carousel-next', 'Next', '›', 1)

    var dots = document.createElement('div')
    dots.className = 'carousel-dots'
    slides.forEach(function (slide, i) {
      var d = document.createElement('button')
      d.className = 'carousel-dot'
      d.setAttribute('aria-label', 'Slide ' + (i + 1))
      d.addEventListener('click', function () { go(i) })
      dots.appendChild(d)
    })
    carousel.appendChild(dots)

    // Phones play a video inline only when asked to
    track.querySelectorAll('video').forEach(function (v) {
      v.setAttribute('playsinline', '')
      v.muted = true
    })

    var mark = function () {
      var i = current()
      dots.querySelectorAll('.carousel-dot').forEach(function (d, k) {
        d.classList.toggle('active', k === i)
      })
      slides.forEach(function (slide, k) {
        var v = slide.querySelector('video')
        if (!v) return
        if (k === i) v.play().catch(function () {})
        else v.pause()
      })
    }
    track.addEventListener('scroll', mark)
    mark()

    carousel.tabIndex = 0
    carousel.addEventListener('keydown', function (e) {
      if (e.key === 'ArrowLeft') { go(current() - 1); e.preventDefault() }
      if (e.key === 'ArrowRight') { go(current() + 1); e.preventDefault() }
    })
  })
})
