
#ifndef _COREOBJECT_H_
#define _COREOBJECT_H_

//Any class derived from _Virtual_Object can't be instanced
class _Virtual_Object{
private:
	_Virtual_Object();
	_Virtual_Object(const _Virtual_Object&);
};

/* _Sealed_Object itself can't be instanced.
 * Any class which directively private inherited from _Sealed_object can be
 * instanced, but its subclass cannot be instanced anymore.
 * For example:
 * class CanInstance_Object : private _Sealed_Object{
 *     XXXXXXXXXXXXXX;
 * };
 * CanInstance_Object can be instanced, but 
 * class Any_Object : public CanInstance_Object{
 *     XXXXXXXXXXXXXX;
 * };
 * Any_Object can't be instanced any more.
 */
class _Sealed_Object{
protected:
	_Sealed_Object();
	_Sealed_Object(const _Sealed_Object&);
};

#endif //!_COREOBJECT_H_
