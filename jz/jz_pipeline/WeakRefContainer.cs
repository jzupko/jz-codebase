using System;
using System.Collections.Generic;
using System.Text;

namespace jz
{
    /// <summary>
    /// A container that maintains a collection of weak references by id.
    /// </summary>
    /// <typeparam name="U">The id type.</typeparam>
    /// <typeparam name="T">The type of object to store weak references to.</typeparam>
    /// <remarks>
    /// This is useful as a global cache for objects by id as objects can still be 
    /// garbage collected even if they are stored in the collection.
    /// </remarks>
    public sealed class WeakRefContainer<U, T> 
        where T : class
    {
        #region Private members
        private readonly U mNullId;
        private Dictionary<U, WeakReference> mCollection = new Dictionary<U, WeakReference>();

        private struct RetrieveQueueEntry
        {
            public RetrieveQueueEntry(U aId, Action<T> aAction)
            {
                Id = aId;
                Action = aAction;
            }

            public U Id;
            public Action<T> Action;
        }
        private LinkedList<RetrieveQueueEntry> mRetrieveQueue = new LinkedList<RetrieveQueueEntry>();

        private T _Retrieve(U aId)
        {
            if (mCollection.ContainsKey(aId))
            {
                return (T)mCollection[aId].Target;
            }

            return null;
        }

        private void _TickRetrieveQueue()
        {
            for (LinkedListNode<RetrieveQueueEntry> e = mRetrieveQueue.First; e != null; )
            {
                T entry = _Retrieve(e.Value.Id);
                if (entry != null)
                {
                    e.Value.Action(entry);
                    LinkedListNode<RetrieveQueueEntry> t = e;
                    e = e.Next;
                    mRetrieveQueue.Remove(t);
                }
                else
                {
                    e = e.Next;
                }
            }
        }
        #endregion

        /// <summary>
        /// Constructs a new weak reference container.
        /// </summary>
        /// <param name="aNullId">The id that represents a null value. i.e. string.Empty</param>
        public WeakRefContainer(U aNullId)
        {
            mNullId = aNullId;
        }

        /// <summary>
        /// Adds a new entry.
        /// </summary>
        /// <param name="aId">The id of the entry.</param>
        /// <param name="aEntry">The entry to add.</param>
        /// <remarks>
        /// Note that unlike many containers, if an entry already exists with the given id, it is replaced
        /// with the entry being added (vs. throwing an exception).
        /// </remarks>
        public void Add(U aId, T aEntry)
        {
            _TickRetrieveQueue();

            if (!aId.Equals(mNullId))
            {
                if (mCollection.ContainsKey(aId))
                {
                    mCollection[aId].Target = aEntry;
                }
                else
                {
                    mCollection.Add(aId, new WeakReference(aEntry));
                }
            }
        }

        /// <summary>
        /// Removes the entry with the given id.
        /// </summary>
        /// <param name="aId">The id of an entry to remove.</param>
        /// <remarks>
        /// Id the id is equal to the null id set at creation or an entry isn't in the collection
        /// with the given id, nothing happens.
        /// </remarks>
        public void Remove(U aId)
        {
            if (!aId.Equals(mNullId))
            {
                mCollection.Remove(aId);
            }
        }

        /// <summary>
        /// Immediately returns the entry with the given id.
        /// </summary>
        /// <param name="aId">The id of the entry to return.</param>
        /// <returns>The entry or null if the entry is not in the collection.</returns>
        public T Retrieve(U aId)
        {
            _TickRetrieveQueue();

            return _Retrieve(aId);
        }

        /// <summary>
        /// Queues an entry for retrieval, calling aAction when the entry is present.
        /// </summary>
        /// <param name="aId">The id of the entry to retrieve.</param>
        /// <param name="aAction">The delegate to call with the entry when it is present.</param>
        /// <remarks>
        /// This is useful when entries in the collection need to acquire other entries in the collection
        /// but it is not defined what the order of insertion of those entries will be. The queue is 
        /// iterated every time WeakRefContainer.Add() or WeakRefContainer.Retrieve() is called.
        /// </remarks>
        public void QueueForRetrieval(U aId, Action<T> aAction)
        {
            if (mCollection.ContainsKey(aId))
            {
                aAction((T)mCollection[aId].Target);
            }
            else
            {
                mRetrieveQueue.AddLast(new RetrieveQueueEntry(aId, aAction));
            }
        }

        public void Tick()
        {
            _TickRetrieveQueue();
        }

        /// <summary>
        /// Updates an existing entry to a new id.
        /// </summary>
        /// <param name="aNewId">The new id of the entry.</param>
        /// <param name="aEntry">The entry to update.</param>
        /// <param name="aOldId">The existing id of the entry.</param>
        /// <remarks>
        /// If an entry with aOldId is not present in the collection, this method is
        /// the same as calling WeakRefContainer.Add().
        /// </remarks>
        public void Update(U aNewId, T aEntry, U aOldId)
        {
            if (!aOldId.Equals(mNullId))
            {
                Remove(aOldId);
            }

            Add(aNewId, aEntry);
        }




    }
}
