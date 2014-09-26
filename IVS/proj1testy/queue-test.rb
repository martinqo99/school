# Autor : Michal Lukac, xlukac05@stud.fit.vutbr.cz, 29.2.2011
# Info  : Tento skript sluzi ako projekt do predmetu IVS na VUT FIT.
#         Ulohou tohto skriptu je testovanie fronty a jej funkci
#         queue, dequeue, enqueue a peek.

require 'test/unit'
require 'queue'

class QueueTest < Test::Unit::TestCase
  # spousti se pred kazdym testem, tj. nova fronta vznika pro kazdy test
  def setup
    @queue = Queue.new
  end

  # enqueue se netestuje zvlast, protoze je nutnou prerekvizitou
  # pro funkcnost ostatnich testu

  # Test na zistenie prveho prvku na prazdnej fronte.
  def test_peekOnEmptyQueue
    assert_equal( nil, @queue.peek )
  end

  # Test na zistenie prveho prvku na prazdnej fronte.
  def test_peekOnNonEmptyQueue
    @queue.enqueue(100)
    assert_equal( 100 , @queue.peek )
  end

  # Test vybratia prvku na prazdnej fronte.
  def test_dequeueOnEmptyStack
    assert_equal( nil , @queue.dequeue )
  end

  # Test vybratka prvku z fronty.
  def test_dequeueOnNonEmptyStack
    @queue.enqueue(100)
    assert_equal( 100 , @queue.dequeue )
  end

  # Test ci je fronta prazdna.
  def test_isEmptyOnEmptyQueue
    assert_equal( true , @queue.empty? )
  end

  # Test ci je fronta prazdna pokial obsahuje polozky. 
  def test_isEmptyOnNonEmptyQueue
    @queue.enqueue(100)
    assert_equal( false , @queue.empty? )
  end

  # vlozeni a vybrani alespon dvou elementu a test na poradi
  def test_orderOfElements
    @queue.enqueue(100)
    @queue.enqueue(200)
    assert_equal( 100, @queue.dequeue )
    assert_equal( 200, @queue.dequeue )
  end
end
